tftbin: tft.o main.o
	gcc -o tftbin main.o tft.o -l wiringPi

tft.o: tft.c
	gcc -Wall -c tft.c

main.o: main.c
	gcc -Wall -c main.c

clean:
	rm main.o tft.o

#gcc -Wall -c main.c tft.c
#gcc -o tftb main.o tft.o -l wiringPi
#rm main.o tft.o
