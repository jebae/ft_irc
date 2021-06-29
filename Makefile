# path
CLIENT_DIR = client
SERVER_DIR = server

all:
	$(MAKE) -C $(CLIENT_DIR)
	$(MAKE) -C $(SERVER_DIR)

clean:
	$(MAKE) -C $(CLIENT_DIR) clean
	$(MAKE) -C $(SERVER_DIR) clean

fclean:
	$(MAKE) -C $(CLIENT_DIR) fclean
	$(MAKE) -C $(SERVER_DIR) fclean

re: fclean all

.PHONY: all clean fclean re