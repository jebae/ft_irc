#ifndef TYPE_H
# define TYPE_H

# define MSG_TYPE_NICK					1
# define MSG_TYPE_NICKACK				2
# define MSG_TYPE_CREATE_CHANNEL		3
# define MSG_TYPE_CREATE_CHANNELACK		4
# define MSG_TYPE_LEAVE					5
# define MSG_TYPE_LEAVEACK				6
# define MSG_TYPE_REMOVE_CHANNEL		7
# define MSG_TYPE_REMOVE_CHANNELACK		8
# define MSG_TYPE_JOIN					9
# define MSG_TYPE_JOINACK				10
# define MSG_TYPE_CHANNEL_CHAT			11
# define MSG_TYPE_DIRECT_CHAT			12
# define MSG_TYPE_CHAT_RELAY			13
# define MSG_TYPE_CHATACK				14
# define MSG_TYPE_WHERE_AM_I			15
# define MSG_TYPE_WHERE_AM_IACK			16

# define MAX_NICK_LEN					15
# define MAX_CHANNEL_NAME_LEN			30

typedef char				t_int8;
typedef unsigned char		t_uint8;
typedef short				t_int16;
typedef unsigned short		t_uint16;
typedef int					t_int32;
typedef unsigned int		t_uint32;
typedef long long			t_int64;
typedef unsigned long long	t_uint64;

#pragma pack(push, 1)

typedef struct	s_msg_hdr
{
	t_uint8		type;
	t_uint64	size;
}				t_msg_hdr;

typedef struct	s_chat_hdr
{
	t_uint8		type;
	t_uint64	size;
	t_uint64	content_offset;
}				t_chat_hdr;

typedef union	u_hdr
{
	t_msg_hdr	msg;
	t_chat_hdr	chat;
}				t_hdr;

#pragma pack(pop)

#endif