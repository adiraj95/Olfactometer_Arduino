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
int valve1_state = 0;
int valve2_state = 0;
int valve3_state = 0;
int valve4_state = 0;
int valve5_state = 0;
int final_vavlve_state = 0;
int seq_length = 0;
int count = 0;


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
    while (seq_length < 1){
          recvWithStartEndMarkers();
          if(newData == true && seq_length == 0) {seq_length = atoi(receivedChars);newData = false;}   //odor IDs mapp on to valve IDs
          
    }
    Serial.print("seq_length =");
    Serial.println(seq_length);
    int on_durations[seq_length]; 
    int off_durations[seq_length]; 
    int odor_IDs[seq_length];  
    Serial.print("on_durations =");
    Serial.println(on_durations[seq_length]);
    
   for (count=0; count < 3*seq_length;){                  
        
        recvWithStartEndMarkers();
        if(newData == true && count < seq_length) {on_durations[count] = atoi(receivedChars);count = count+1;Serial.print("on_durations ="); Serial.println(on_durations[count]); newData = false;}
        if(newData == true && count > seq_length-1 && count <seq_length*2) {off_durations[count] = atoi(receivedChars);count = count+1;Serial.print("off_durations =");Serial.println(off_durations[count]);newData = false;}
        if(newData == true && count > (2*seq_length)-1) {odor_IDs[count] = atoi(receivedChars);count = count+1;Serial.print("odor_IDs =");Serial.println(odor_IDs[count]);newData = false;}
        
    }

    //char valve_rise = sprintf("valve","%02d",odor_IDs[1]);
    
   
   //if (odor_IDs == 1) {
    digitalWrite(odor_IDs[1]+1, HIGH);   // turn the LED on (HIGH is the voltage level)
    delay(on_durations[1]);               // wait for a second
    digitalWrite(odor_IDs[1]+1, LOW);    // turn the LED off by making the voltage LOW
    delay(off_durations[1]);
  
    
    seq_length = 0;
    

     
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

