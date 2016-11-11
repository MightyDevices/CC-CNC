/*
 * err.h
 *
 *  Created on: 11-06-2013
 *      Author: Tomek
 */


#ifndef SYS_ERR_H_
#define STS_ERR_H_

/* error codes */
#define EOK                     			(0)
/* fatal error */
#define EFATAL                  			(-1)
/* device busy */
#define EBUSY                  	 			(-2)
/* not connected */
#define ENOCONNECT              			(-3)
/* data too large */
#define ETOOLARGE               			(-4)

/* number format error */
#define ENUM_FORMAT                         (-10)
/* no closing comma bracket */
#define ECOMMENT_BRACKET                    (-11)
/* no command letter */
#define ENO_LETTER					        (-12)
/* line too long */
#define ELINE_TOO_LONG				        (-13)
/* unknown letter code */
#define EUNS_LETTER_CODE			        (-14)
/* unsupported g code */
#define EUNS_GCODE					        (-15)
/* unsupported m code */
#define EUNS_MCODE					        (-16)
/* unsupported m code */
#define EMODAL_CONFLICT			            (-17)
/* duplicate parameter word in a single line */
#define EWORD_DUPLICATE			            (-18)
/* no words required by the commands were provided */
#define EWORD_MISSING				        (-19)
/* single word provided for multiple commands */
#define EWORD_CONFLICT				        (-20)
/* excessive words found */
#define EWORD_EXCESS				        (-21)
/* word value not permitted */
#define EWORD_VALUE			                (-22)
/* line number too large */
#define EINVALID_LINE_NUM			        (-23)
/* error during setting feed mode */
#define EEXEC_FEED_MODE			            (-24)
/* error diring setting feed rate */
#define EEXEC_FEED_RATE			            (-25)
/* error during spindle speed adjustment */
#define EEXEC_SPINDLE_SPEED		            (-26)
/* error during tool change execution */
#define EEXEC_TOOL					        (-27)
/* error during spindle on/off execution */
#define EEXEC_SPINDLE_ONOFF		            (-28)
/* error during coolant on/off execution */
#define EEXEC_COOLANT				        (-29)
/* error during overrides on/off execution */
#define EEXEC_OVERRIDES			            (-30)
/* error during overrides on/off execution */
#define EEXEC_DWELL				            (-31)
/* error during setting active plane */
#define EEXEC_PLANE				            (-32)
/* error during setting uints */
#define EEXEC_UNITS				            (-33)
/* error during cutter radius compensation */
#define EEXEC_CUT_RADIUS_COMP		        (-34)
/* error during tool length offset setting */
#define EEXEC_TOOL_LENGTH_OFFS		        (-35)
/* error during selection of coordinate system */
#define EEXEC_COORD_SELECTION		        (-36)
/* error during changing the path control mode */
#define EEXEC_PATH_CTRL			            (-37)
/* error during selecting the distance mode */
#define EEXEC_DISTANCE_MODE		            (-38)
/* error during setting retract modes */
#define EEXEC_RETRACT				        (-39)
/* error during homing */
#define EEXEC_HOMING				        (-40)
/* error during changing coordinate system data */
#define EEXEC_COORD_DATA_CHANGE	            (-41)
/* error during changing coordinate system data */
#define EEXEC_AXIS_OFFSET			        (-42)
/* error during motion */
#define EEXEC_MOTION				        (-43)
/* error during stopping */
#define EEXEC_STOP					        (-44)


#endif

