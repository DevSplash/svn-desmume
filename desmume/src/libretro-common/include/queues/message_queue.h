/* Copyright  (C) 2010-2016 The RetroArch team
 *
 * ---------------------------------------------------------------------------------------
 * The following license statement only applies to this file (message_queue.h).
 * ---------------------------------------------------------------------------------------
 *
 * Permission is hereby granted, free of charge,
 * to any person obtaining a copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#ifndef __LIBRETRO_SDK_MSG_QUEUE_H
#define __LIBRETRO_SDK_MSG_QUEUE_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct msg_queue msg_queue_t;

/**
 * msg_queue_new:
 * @size              : maximum size of message
 *
 * Creates a message queue with maximum size different messages.
 *
 * Returns: NULL if allocation error, pointer to a message queue
 * if successful. Has to be freed manually.
 **/
msg_queue_t *msg_queue_new(size_t size);

/**
 * msg_queue_push:
 * @queue             : pointer to queue object
 * @msg               : message to add to the queue
 * @prio              : priority level of the message
 * @duration          : how many times the message can be pulled
 *                      before it vanishes (E.g. show a message for
 *                      3 seconds @ 60fps = 180 duration).
 *
 * Push a new message onto the queue.
 **/
void msg_queue_push(msg_queue_t *queue, const char *msg,
      unsigned prio, unsigned duration);

/**
 * msg_queue_pull:
 * @queue             : pointer to queue object
 *
 * Pulls highest priority message in queue.
 *
 * Returns: NULL if no message in queue, otherwise a string
 * containing the message.
 **/
const char *msg_queue_pull(msg_queue_t *queue);

/**
 * msg_queue_clear:
 * @queue             : pointer to queue object
 *
 * Clears out everything in the queue.
 **/
void msg_queue_clear(msg_queue_t *queue);

/**
 * msg_queue_free:
 * @queue             : pointer to queue object
 *
 * Frees message queue..
 **/
void msg_queue_free(msg_queue_t *queue);

#ifdef __cplusplus
}
#endif

#endif
