# ft_irc

ft_irc is client and server program of internet relay chat. Main idea is based on IRC in RFC.
Server and client use `select` function to multiplex IO. Chat feature supports direct and broadcast type. For more faster performance, server uses hash data structure not checking all users who use this protocol.

<br><br>

## Requirements

```
# download libft
git clone https://github.com/jebae/libft.git

# directory structure
root/
 libs/
  libft
```

<br><br>

## Installation

```
# server and client in one command
$> make
```

<br/><br/>

## Usage

### server

```
$> pwd
ft_irc/server
$> ./server [port number]
```

<br/>

### client

```
$> pwd
ft_irc/client
$> ./client [server port number]
```

<br/><br/>

### Client commands

- `/nick [nickname]`: set my nickname

- `/create_channel [channel_name]`

- `/remove_channel [channel_name]`

- `/join [channel_name]`

- `/leave`: leave joined channel.

- `/msg [content]`: broadcast chat to joined channel.

- `/msg #[nick] [content]`: chat direct with user named [nick].

- `/who`: show users in joined channel.

- `/where_am_i`: show joined channel.

- `/channels`: show channel list.

- `/disconnect` or Ctrl-C