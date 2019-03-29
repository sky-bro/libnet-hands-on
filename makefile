all:
	gcc forgetcp.c -o ./build/forgetcp -lnet
	gcc forgeudp.c -o ./build/forgeudp -lnet
	gcc forgearp.c -o ./build/forgearp -lnet
clean:
	rm packetforger
