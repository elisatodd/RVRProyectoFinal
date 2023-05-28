game:
	# make -f client.mk
	# make -f server.mk
	make -f game.mk

clean:
	make clean -f client.mk
	make clean -f server.mk