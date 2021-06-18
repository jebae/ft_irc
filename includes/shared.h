#ifndef TYPE_H
# define TYPE_H

# define MSG_TYPE_NICK					1
# define MSG_TYPE_NICKACK				2
# define MSG_TYPE_CREATE_CHANNEL		3
# define MSG_TYPE_CREATE_CHANNELACK		4

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

#pragma pack(pop)

#endif