all: semaphone.c semaphone_write.c
	gcc -o control semaphone.c
	gcc -o main semaphone_write.c

clean:
	rm control
	rm main
