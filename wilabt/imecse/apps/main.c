/** 
	Example source file:
        The sensing is configured to WLAN_G mode, scan through all 14 channels and print the result out on stdout
        Press control+C to stop the sensing engine
	lwei@intec.ugent.be
*/

// include header files required by imec sensing engine
#include "sensing.h"
#include "types.h"

#include <signal.h> // SIGINT .. 


/*----------------------------------------------------------------------------*/
/* sensing engine running state management.                                   */
/*----------------------------------------------------------------------------*/

typedef enum{
  seState_Stop    = 0,
  seState_Running = 1
} seState_t;

static seState_t seState = seState_Stop;

seStopHandler(int _dummy_){
  seState = seState_Stop;
  return;
}

/*----------------------------------------------------------------------------*/
/* The main function     		                                      */
/*----------------------------------------------------------------------------*/

int main (int argc, char* argv[])
{
    printf("\nGreetings from imec sensing engine...\n");
    // Define variables
    se_t se_1_h;
    struct se_config_s my_se_config; // the struct that contains imec sensing engine configuration
    float fft_result[4*128]; // array used to store imec sensing engine result
    int spider, frontend; // variables to store spider ID, and radio frontend ID, for WARP radio board, the frontend should be asigned to 0, read the README for more info
    int i; // used for loop
    
    // Process argument 
    if (argc==3) {
        spider = atoi(argv[1]); // the first arg is spider ID
        frontend = atoi(argv[2]); // the second arg is the frontend ID
    }else{
    	printf("ERROR: Incorrect argument \n\
\tUsage: %s spider frontend\n\
\tExample %s 129 1\n\
\tThe spider and frontend are spicified in decimal \n\
\tMore info regarding to frontend and spider ID see the README\n",argv[0],argv[0]);
    	return -1;
    }

    // Open the sensing engine hardware
    se_1_h = se_open(spider, frontend);
    int result = se_init(se_1_h, &my_se_config);

    // Check if the sensing engine is opened successfully 
    printf("Initialization [0 invalid, 1 valid]: %i\n", result);
    assert(result == 1);

    // Assign configuration (scan 14 channels for WLAN_G standard) to the sensing engine 
    my_se_config.first_channel = 1;
    my_se_config.last_channel = 12;
    my_se_config.fft_points = 128;
    my_se_config.fe_gain = 80; // Max gain = 100
    my_se_config.se_mode = WLAN_G; // valide mode: FFT_SWEEP, ZIGBEE, BLUETOOTH, WLAN_G...

    //Check if the configuration above is valid for the sensing engine
    result = se_check_config(se_1_h, my_se_config);printf("Configuration check [0 invalid, 1 valid]: %i\n", result);
    assert(result == 1);

    //Configure the sensing engine
    result = se_configure(se_1_h, my_se_config, 1);printf("Configuration setup [0 invalid, 1 valid]: %i\n", result);
    assert(result != 0);

    //Register stop handeler 
    //signal(SIGINT,(__sighandler_t) seStopHandler); 
    //signal(SIGTERM,(__sighandler_t) seStopHandler);
    
    //Change the seState to running
    seState = seState_Running;

    //Start the sensing engine
    fprintf(stdout,"starting ... \n") ;   
    se_start_measurement(se_1_h);
    //Enter the loop 
    while (seState == seState_Running) {
        // Read result from the sensing engine
        se_get_result(se_1_h, fft_result);
        for (i=0;i<(my_se_config.last_channel - my_se_config.first_channel + 1);i++) {
            fprintf(stdout,"%d,%3.0f\n", i+1, fft_result[i]);
        }
    }
    
    // Close the sensing engine
    se_stop_measurement(se_1_h);usleep(2000);
    if (se_get_status(se_1_h)) se_close(se_1_h);

    return 0;
}
