/*
 * xfrm_output.c - Common IPsec encapsulation code.
 *
 * Copyright (c) 2007 Herbert Xu <herbert@gondor.apana.org.au>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#include <linux/netfilter.h>
#include <linux/skbuff.h>
#include <net/xfrm.h>

#include "ax8netfilter.h"

static int ax8netfilter_xfrm_state_check_space(struct xfrm_state *x, struct sk_buff *skb)
{
	struct dst_entry *dst = skb->dst;
	int nhead = dst->header_len + LL_RESERVED_SPACE(dst->dev)
		- skb_headroom(skb);
	int ntail = dst->dev->needed_tailroom - skb_tailroom(skb);

	if (nhead <= 0) {
		if (ntail <= 0)
			return 0;
		nhead = 0;
	} else if (ntail < 0)
		ntail = 0;

	return pskb_expand_head(skb, nhead, ntail, GFP_ATOMIC);
}

static int ax8netfilter_xfrm_output_one(struct sk_buff *skb, int err)
{
	struct dst_entry *dst = skb->dst;
	struct xfrm_state *x = dst->xfrm;
	struct net *net = xs_net(x);

	if (err <= 0)
		goto resume;

	do {
		err = ax8netfilter_xfrm_state_check_space(x, skb);
		if (err) {
			XFRM_INC_STATS(net, LINUX_MIB_XFRMOUTERROR);
			goto error_nolock;
		}

		err = x->outer_mode->output(x, skb);
		if (err) {
			XFRM_INC_STATS(net, LINUX_MIB_XFRMOUTSTATEMODEERROR);
			goto error_nolock;
		}

		spin_lock_bh(&x->lock);
		err = xfrm_state_check_expire(x);
		if (err) {
			XFRM_INC_STATS(net, LINUX_MIB_XFRMOUTSTATEEXPIRED);
			goto error;
		}

		if (x->type->flags & XFRM_TYPE_REPLAY_PROT) {
			XFRM_SKB_CB(skb)->seq.output = ++x->replay.oseq;
			if (unlikely(x->replay.oseq == 0)) {
				XFRM_INC_STATS(net, LINUX_MIB_XFRMOUTSTATESEQERROR);
				x->replay.oseq--;
				xfrm_audit_state_replay_overflow(x, skb);
				err = -EOVERFLOW;
				goto error;
			}
			if (xfrm_aevent_is_on(net))
				ax8netfilter_xfrm_replay_notify(x, XFRM_REPLAY_UPDATE);
		}

		x->curlft.bytes += skb->len;
		x->curlft.packets++;

		spin_unlock_bh(&x->lock);

		err = x->type->output(x, skb);
		if (err == -EINPROGRESS)
			goto out_exit;

resume:
		if (err) {
			XFRM_INC_STATS(net, LINUX_MIB_XFRMOUTSTATEPROTOERROR);
			goto error_nolock;
		}

		if (!(skb->dst = dst_pop(dst))) {
			XFRM_INC_STATS(net, LINUX_MIB_XFRMOUTERROR);
			err = -EHOSTUNREACH;
			goto error_nolock;
		}
		dst = skb->dst;
		x = dst->xfrm;
	} while (x && !(x->outer_mode->flags & XFRM_MODE_FLAG_TUNNEL));

	err = 0;

out_exit:
	return err;
error:
	spin_unlock_bh(&x->lock);
error_nolock:
	kfree_skb(skb);
	goto out_exit;
}

int ax8netfilter_xfrm_output_resume(struct sk_buff *skb, int err)
{
	while (likely((err = ax8netfilter_xfrm_output_one(skb, err)) == 0)) {
		nf_reset(skb);

		err = skb->dst->ops->local_out(skb);
		if (unlikely(err != 1))
			goto out;

		if (!skb->dst->xfrm)
			return dst_output(skb);

		err = nf_hook(skb->dst->ops->family,
			      NF_INET_POST_ROUTING, skb,
			      NULL, skb->dst->dev, ax8netfilter_xfrm_output2);
		if (unlikely(err != 1))
			goto out;
	}

	if (err == -EINPROGRESS)
		err = 0;

out:
	return err;
}
