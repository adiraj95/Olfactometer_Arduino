// Code to allow arduino to control and manipulate valves on v2 of Yoshi's olfactormeter. 
// Written by Adithya Rajagopalan on 04.17.19

// Defining variables and pin numbers

const byte numChars = 32;
char receivedChars[numChars];

boolean newData = false;

const int valve1 = 2;
const int valve2 = 3;
const int valve3 = 4;
const int valve4 = 5;
const int valve5 = 6;
const int final_valve = 13;

int trig_state = 0;
int init_wait_time = 0;
int seq_length = 0;
int count = 0;
int on_count = 0;
int off_count = 0;
int od_count = 0;


// the setup routine runs once when you press reset:
void setup() {                
  // initialize the digital pin as an output.
  pinMode(valve1, OUTPUT);
  pinMode(valve2, OUTPUT);
  pinMode(valve3, OUTPUT);
  pinMode(valve4, OUTPUT);
  pinMode(valve5, OUTPUT);
  pinMode(final_valve, OUTPUT);     
  Serial.begin(9600);
  Serial.println("<Arduino is ready>");
  
}
// the loop routine runs over and over again forever:
void loop() {


      // receiving serial info about the length of the sequence of pulses that is to be delivered
      while (seq_length < 1){
          recvWithStartEndMarkers();
          if(newData == true && seq_length == 0) {seq_length = atoi(receivedChars);newData = false;}   //odor IDs mapp on to valve IDs
        
      }
      Serial.print("seq_length =");
      Serial.println(seq_length);
      int on_durations[seq_length]; 
      int off_durations[seq_length]; 
      int odor_IDs[seq_length];  

      // receiving serial info about the train of pulses to be delivered. This fills up on_durations, off_durations, odor_IDs and init_wait_time
  
      while (count < (3*seq_length)+1){                  
      
          recvWithStartEndMarkers();
          if(newData == true && count < seq_length) {on_durations[on_count+1] = atoi(receivedChars);on_count = on_count+1; count = count+1; Serial.print("on_durations ="); Serial.println(on_durations[on_count]); newData = false;}
          if(newData == true && count > seq_length-1 && count <(seq_length*2)) {off_durations[off_count+1] = atoi(receivedChars);off_count = off_count+1;count = count+1; Serial.print("off_durations ="); Serial.println(off_durations[off_count]);newData = false;}
          if(newData == true && count > (2*seq_length)-1 && count <(seq_length*3)) {odor_IDs[od_count+1] = atoi(receivedChars);od_count = od_count+1;count = count+1; Serial.print("odor_IDs ="); Serial.println(odor_IDs[od_count]);newData = false;}
          if(newData == true && count > (3*seq_length)-1) {init_wait_time = atoi(receivedChars); count = count+1;  Serial.print("init_wait_time ="); Serial.println(init_wait_time);newData = false;}
      
      }

  
      // Triggering high and low states in appropriate pins based on on_durations, off_durations, odor_IDs and init_wait_time
      
      for (int i = 0; i<seq_length; i++) {
          if (i == 0) { delay(init_wait_time);}
          digitalWrite(odor_IDs[i+1]+1, HIGH);   // turn the LED on (HIGH is the voltage level)
          delay(on_durations[i+1]);               // wait for a second
          digitalWrite(odor_IDs[i+1]+1, LOW);    // turn the LED off by making the voltage LOW
          delay(off_durations[i+1]);
      }
      Serial.print("Train complete");

      // resetting variables for next loop
      
      seq_length = 0;
      count = 0;
      on_count = 0;
      off_count = 0;
      od_count = 0;
      for (int i=0; i<seq_length; i++){on_durations[i] = 0;}
      for (int i=0; i<seq_length; i++){off_durations[i] = 0;}
      for (int i=0; i<seq_length; i++){odor_IDs[i] = 0;}
      init_wait_time = 0;

     
}


//SERIAL READ FUNCTIONS

void recvWithStartEndMarkers() {
    static boolean recvInProgress = false;
    static byte ndx = 0;
    char startMarker = '<';
    char endMarker = '>';
    char rc;
 
    while (Serial.available() > 0 && newData == false) {
        rc = Serial.read();

        if (recvInProgress == true) {
            if (rc != endMarker) {
                receivedChars[ndx] = rc;
                ndx++;
                if (ndx >= numChars) {
                    ndx = numChars - 1;
                }
            }
            else {
                receivedChars[ndx] = '\0'; // terminate the string
                recvInProgress = false;
                ndx = 0;
                newData = true;
                
            }
        }

        else if (rc == startMarker) {
            recvInProgress = true;
        }
    }
}

void showNewData() {
    if (newData == true) {
        Serial.print("This just in ... ");
        Serial.println(receivedChars);
        newData = false;
        
    }
}


