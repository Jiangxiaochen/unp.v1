/*
 * =====================================================================================
 *    Description: pseudo code for rdt2.2 pack may be broken
 *        Version:  1.0
 *        Created:  09/30/2018 12:47:34 PM
 *         Author:  xiaochen.jiang (kl), jiangxch.hn@gmail.com
 * =====================================================================================
 */

void sender()
{
  wait_cal0:
	wait(rdt_send(data));
	sndpkt = make_pkt(0, data, checksum);
	udt_send(snd_pkt);
  wait_ack0:
	wait(rdt_rcv(rcvpkt));
	if (corrupt(rcvpkt) || isAck(rcvpkt, 1))
		udt_send(snd_pkt), goto wait_ack0;
  wait_cal1:
	wait(rdt_send(data));
	sndpkt = make_pkt(1, data, checksum);
	udt_send(snd_pkt);
  wait_ack1:
	wait(rdt_rcv(rcvpkt));
	if (corrupt(rcvpkt) || isAck(rcvpkt, 0))
		udt_send(snd_pkt), goto wait_ack1;
	goto wait_cal0;
}

void recver()
{
  wait_rcv0:
	wait(rdt_rcv(rcvpkt));
	if (corrupt(rcvpkt) || has_seq1(rcvpkt)) {
		sndpkt = make_pkt(ACK, 1, checksum);
		udt_send(sndpkt);
		goto wait_rcv0;
	}
	extract(rcvpkt, data);
	deliver_data(data);
	sndpkt = make_pkt(ACK, 0, checksum);
	udt_send(sndpkt);
  wait_rcv1:
	wait(rdt_rcv(rcvpkt));
	if (corrupt(rcvpkt) || has_seq0(rcvpkt)) {
		sndpkt = make_pkt(ACK, 0, checksum);
		udt_send(sndpkt);
		goto wait_rcv1;
	}
	extract(rcvpkt, data);
	deliver_data(data);
	sndpkt = make_pkt(ACK, 1, checksum);
	udt_send(sndpkt);
	goto wait_rcv0;
}
