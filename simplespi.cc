#include <node.h>
#include <v8.h>

using namespace v8;

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>

#define ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))

static void errormsg(const char *s)
{
	ThrowException(Exception::Error(String::New(s)));
}

const char* ToCString(const v8::String::Utf8Value& value) {
  return *value ? *value : "<string conversion failed>";
}

static uint8_t mode;
static uint8_t bits = 8;
static uint32_t speed = 500000;
static uint16_t delay;


// Send Function
Handle<Value> Send(const Arguments& args) {
    HandleScope scope;

    int ret = 0;
	int fd;



    //Hexstring as first Argument. Convert it to Hex-Array
    String::Utf8Value str(args[0]);
    const char* hexinput = ToCString(str);

    int len = strlen(hexinput);

    uint8_t tx[len / 2];
    uint8_t rx[len / 2];
    size_t count = 0;

    for(count = 0; count < sizeof(tx)/sizeof(tx[0]); count++) {
        sscanf(hexinput, "%2hhx", &tx[count]);
        hexinput += 2 * sizeof(char);
    }

    /* debug */
    /*
    for(count = 0; count < sizeof(tx)/sizeof(tx[0]); count++)
      printf("%02x:", tx[count]);
    printf("\n");
    */


    // Open Device and Check
    const char* device;
    String::Utf8Value str2(args[1]);

    if(args[1]->IsString()) {
        //if second Parameter is defined, use it as device
        device = (char*) ToCString(str2);
    } else {
        device = "/dev/spidev0.0"; //Standard Device
    }


	fd = open(device, O_RDWR);
	if (fd < 0)
		errormsg("can't open device");

	ret = ioctl(fd, SPI_IOC_WR_MODE, &mode);
	if (ret == -1)
		errormsg("can't set spi mode");

	ret = ioctl(fd, SPI_IOC_RD_MODE, &mode);
	if (ret == -1)
		errormsg("can't get spi mode");

	ret = ioctl(fd, SPI_IOC_WR_BITS_PER_WORD, &bits);
	if (ret == -1)
		errormsg("can't set bits per word");

	ret = ioctl(fd, SPI_IOC_RD_BITS_PER_WORD, &bits);
	if (ret == -1)
		errormsg("can't get bits per word");

	ret = ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed);
	if (ret == -1)
		errormsg("can't set max speed hz");

	ret = ioctl(fd, SPI_IOC_RD_MAX_SPEED_HZ, &speed);
	if (ret == -1)
		errormsg("can't get max speed hz");



    // Create Transfer Struct
    struct spi_ioc_transfer tr;
    tr.tx_buf = (unsigned long)tx;
    tr.rx_buf = (unsigned long)rx;
    tr.len = ARRAY_SIZE(tx);
    tr.delay_usecs = delay;
    tr.speed_hz = speed;
    tr.bits_per_word = bits;


    // Send SPI-Message
    ret = ioctl(fd, SPI_IOC_MESSAGE(1), &tr);
    if (ret < 1)
        errormsg("can't send spi message");


    //Close Device
	close(fd);



    // Convert Return Values Back to a Hexstring
    unsigned int i;
    char* buf_str = (char*) malloc (2*ARRAY_SIZE(rx)+1);
    char* buf_ptr = buf_str;
    for (i = 0; i < ARRAY_SIZE(rx); i++)
    {
       buf_ptr += sprintf(buf_ptr, "%02X", rx[i]);
    }
    *(buf_ptr + 1) = '\0';



    return scope.Close(String::New(buf_str));
}





/*
    Constructor des Moduls
*/
void init(Handle<Object> exports) {

    exports->Set(String::NewSymbol("send"), FunctionTemplate::New(Send)->GetFunction());

}


NODE_MODULE(simplespi, init)