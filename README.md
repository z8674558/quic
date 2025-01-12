# quicer

QUIC protocol erlang library.

[msquic](https://github.com/microsoft/msquic) NIF binding.

Project Status: WIP (actively), POC quality

API: is not stable, might be changed in the future

# OS Support
| OS      | Status    |
|---------|-----------|
| Linux   | Supported |
| MACOS   | Planned    |
| Windows | TBD       |

# BUILD

## Dependencies

1. OTP22+
1. rebar3
1. cmake3.16+
1. [CLOG](https://github.com/microsoft/CLOG) (required for debug build only)
1. LTTNG2.12 (required for debug build only)

## With DEBUG

Debug build depedency: [CLOG](https://github.com/microsoft/CLOG) 

``` sh
$ rebar3 compile 
# OR
$ make
```

## Without DEBUG

``` sh
$ git submodule update --init --recursive
$ cmake -B c_build -DCMAKE_BUILD_TYPE=Release -DQUIC_ENABLE_LOGGING=OFF && make 
```

# Examples

## Ping Pong server and client

### Server

``` erlang
Port = 4567,
LOptions = [ {cert, "cert.pem"}
           , {key,  "key.pem"}],
{ok, L} = quicer:listen(Port, LOptions),
{ok, Conn} = quicer:accept(L, [], 5000),
{ok, Stm} = quicer:accept_stream(Conn, []),
receive {quic, <<"ping">>, _, _, _, _} -> ok end,
{ok, 4} = quicer:send(Stm, <<"pong">>),
quicer:close_listener(L).
```

### Client

``` erlang
Port = 4567,
{ok, Conn} = quicer:connect("localhost", Port, [], 5000),
{ok, Stm} = quicer:start_stream(Conn, []),
{ok, 4} = quicer:send(Stm, <<"ping">>),
receive {quic, <<"pong">>, _, _, _, _} -> ok end,
ok = quicer:close_connection(Conn).
```


# TEST

``` sh
$ make test
```

# API

All APIs are exported though API MODULE: quicer.erl

## Terminology
| Term       | Definition                                                       |
|------------|------------------------------------------------------------------|
| server     | listen and accept quic connections from clients                  |
| client     | initiates quic connection                                        |
| listener   | Erlang Process owns listening port                               |
| connection | Quic Connection                                                  |
| stream     | Exchanging app data over a connection                            |
| owner      | 'owner' is a process that receives quic events.                  |
|            | 'connection owner' receive events of a connection                |
|            | 'stream owner' receive application data and events from a stream |
|            | 'listener owner' receive events from listener                    |
|            | When owner is dead, related resources would be released          |
| l_ctx      | listener nif context                                             |
| c_ctx      | connection nif context                                           |
| s_ctx      | stream nif context                                               |
|            |                                                                  |

## Connection API

### Start listener (Server)

Start listener on specific port.

``` erlang
quicer:listen(Port, Options) ->
  {ok, Connection} | {error, any()} | {error, any(), ErrorCode::integer()}.
```

note: port binding is done in NIF context, thus you cannot see it from `inet:i()`.


### Close listener (Server)

``` erlang
quicer:close_listener(Listener) -> ok.
```

Gracefully close listener.

### Accept Connection (Server)

``` erlang
quicer:accept(Listener, Options, Timeout) -> 
  {ok, Connection} | {error, any()} | {error, any(), ErrorCode::integer()}.
```

Blocking call to accept new connection.

Caller becomes the owner of new connection.


### Start Connection  (Client)

``` erlang
quicer:connection(Hostname, Port, Options, Timeout) -> 
  {ok, Connection} | {error, any()} | {error, any(), ErrorCode::integer()}.
```

### close_connection

``` erlang
quicer:close_connection(Connection) -> ok.
```

Gracefully Shutdown connection.

## Stream API

### Start stream

``` erlang
quicer:start_stream(Connection, Options) -> 
  {ok, Stream} | {error, any()} | {error, any(), ErrorCode::integer()}.
```

### Accept stream

``` erlang
accept_stream(Connection, Opts, Timeout) -> 
  {ok, Stream} | {error, any()} | {error, any(), ErrorCode::integer()}.
```


Accept stream on a existing connection. 

This is a blocking call.

### Send Data over stream

``` erlang
quicer:send(Stream, BinaryData) -> 
  {ok, Stream} | {error, any()} | {error, any(), ErrorCode::integer()}.
```

Aync send data over stream.


### Passive receive from stream

``` erlang
quicer:recv(Stream, Len) -> 
  {ok, binary()} | {error, any()} | {error, any(), ErrorCode::integer()}.
```

Like gen_tcp:recv, passive recv data from stream.

If Len = 0, return all data in buffer if it is not empty.
            if buffer is empty, blocking for a quic msg from stack to arrive and return all data from that msg.
If Len > 0, desired bytes will be returned, other data would be buffered in proc dict.

Suggested to use Len=0 if caller want to buffer or reassemble the data on its own.

### Shutdown stream

``` erlang
quicer:close_stream(Stream) -> ok.
```

Shutdown stream gracefully.

# License
Apache License Version 2.0


