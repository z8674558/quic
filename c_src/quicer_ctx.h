#ifndef __QUICER_CTX_H_
#define __QUICER_CTX_H_

#include "quicer_nif.h"
#include "quicer_queue.h"

typedef struct
{
  HQUIC Configuration;
  HQUIC Listener;
  HQUIC Connection;
  QUICER_ACCEPTOR_QUEUE *acceptor_queue;
  ErlNifPid listenerPid;
  ErlNifEnv *env;
  ErlNifMutex *lock;
} QuicerListenerCTX;

typedef struct
{
  HQUIC Configuration;
  HQUIC Connection;
  QuicerListenerCTX *l_ctx;
  QUICER_ACCEPTOR_QUEUE *acceptor_queue;
  ACCEPTOR *owner;
  ErlNifMonitor *owner_mon;
  ErlNifEnv *env;
  ErlNifMutex *lock;
} QuicerConnCTX;

typedef struct
{
  QuicerListenerCTX *l_ctx;
  QuicerConnCTX *c_ctx;
  HQUIC Stream;
  ACCEPTOR *owner;
  ErlNifEnv *env; //@todo destruct env
  ErlNifMutex *lock;
} QuicerStreamCTX;

QuicerListenerCTX *init_l_ctx();
void destroy_l_ctx(QuicerListenerCTX *l_ctx);

QuicerConnCTX *init_c_ctx();
void destroy_c_ctx(QuicerConnCTX *c_ctx);

QuicerStreamCTX *init_s_ctx();
void destroy_s_ctx(QuicerStreamCTX *s_ctx);

#endif // __QUICER_CTX_H_
