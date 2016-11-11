/*
 * words.h
 *
 *  Created on: Oct 15, 2016
 *      Author: Tomek
 */

#ifndef WORDS_H_
#define WORDS_H_

/* number of different words */
#define WORD_NUM					19
/* number of axis words */
#define WORD_AXIS_NUM				6

/* word indices */
#define WORD_X						0
#define WORD_Y						1
#define WORD_Z						2
#define WORD_A						3
#define WORD_B						4
#define WORD_C						5
#define WORD_I						6
#define WORD_J						7
#define WORD_K						8
#define WORD_R						9
#define WORD_T						10
#define WORD_F						11
#define WORD_S						12
#define WORD_P						13
#define WORD_N						14
#define WORD_L						15
#define WORD_H						16
#define WORD_D						17
#define WORD_Q						18

/* word flags */
#define WFLAG_X						(1 << WORD_X)
#define WFLAG_Y						(1 << WORD_Y)
#define WFLAG_Z						(1 << WORD_Z)
#define WFLAG_A						(1 << WORD_A)
#define WFLAG_B						(1 << WORD_B)
#define WFLAG_C						(1 << WORD_C)
#define WFLAG_I						(1 << WORD_I)
#define WFLAG_J						(1 << WORD_J)
#define WFLAG_K						(1 << WORD_K)
#define WFLAG_R						(1 << WORD_R)
#define WFLAG_T						(1 << WORD_T)
#define WFLAG_F						(1 << WORD_F)
#define WFLAG_S						(1 << WORD_S)
#define WFLAG_P						(1 << WORD_P)
#define WFLAG_N						(1 << WORD_N)
#define WFLAG_L						(1 << WORD_L)
#define WFLAG_H						(1 << WORD_H)
#define WFLAG_D						(1 << WORD_D)
#define WFLAG_Q						(1 << WORD_Q)
#define WFLAG_XYZ					(WFLAG_X | WFLAG_Y | WFLAG_Z)
#define WFLAG_ABC					(WFLAG_A | WFLAG_B | WFLAG_C)
#define WFLAG_IJK					(WFLAG_I | WFLAG_J | WFLAG_K)
#define WFLAG_AXIS					(WFLAG_XYZ | WFLAG_ABC)


#endif /* WORDS_H_ */
