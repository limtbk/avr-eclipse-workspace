import java.util.regex.Pattern;

import jssc.*;

public class SimpleInteraction {

	/**
	 * @param args
	 */
	public static void main(String[] args) {
		System.out.println("Start");
		
        String[] portNames = SerialPortList.getPortNames("/dev/", Pattern.compile("tty.SLAB_USBtoUART"));
        SerialPort[] serialPorts = new SerialPort[portNames.length];

        for (int i = 0; i < portNames.length; i++) {
            System.out.println(portNames[i]);
            String portName = portNames[i];
       		SerialPort serialPort = new SerialPort(portName);
       		SerialPortReader portReader = new SerialPortReader();
       		portReader.index = i;
       		portReader.serialPort = serialPort;
       		try {
				serialPort.openPort();
	       		int mask = SerialPort.MASK_RXCHAR;
				serialPort.setEventsMask(mask);
	    		serialPort.addEventListener(portReader);
			} catch (SerialPortException e) {
				e.printStackTrace();
			}
			serialPorts[i] = serialPort;
			i++;
        }
        
        System.out.println(serialPorts);

        int[] baudRates = {SerialPort.BAUDRATE_115200, SerialPort.BAUDRATE_57600, SerialPort.BAUDRATE_38400, SerialPort.BAUDRATE_19200, SerialPort.BAUDRATE_9600};

        for (int i = 0; i < 160; i++) {
            for (int baudRate : baudRates) {
                System.out.println(baudRate);
                for (SerialPort serialPort : serialPorts) {
        			try {
    					serialPort.setParams(baudRate, SerialPort.DATABITS_8, SerialPort.STOPBITS_1, SerialPort.PARITY_NONE);
    	                serialPort.writeBytes("AT\n\r".getBytes());
    				} catch (SerialPortException e) {
    					e.printStackTrace();
    				}
        		}
                
                try {
    				Thread.sleep(16);
    			} catch (InterruptedException e) {
    				e.printStackTrace();
    			}
    		}
		}

        for (SerialPort serialPort : serialPorts) {
        	try {
				serialPort.closePort();
			} catch (SerialPortException e) {
				e.printStackTrace();
			}
        }
        
		System.out.println("Finish");

	}

    static class SerialPortReader implements SerialPortEventListener {
        public SerialPort serialPort;
        public int index;
		public void serialEvent(SerialPortEvent event) {
            if(event.isRXCHAR()) {
            	int bytesCount = event.getEventValue(); 
                if(bytesCount >= 1) {
                    try {
                        byte buffer[] = serialPort.readBytes(bytesCount);
                        String rxString = new String(buffer);
                        System.out.print(serialPort.getPortName());
                        System.out.println(rxString);
                    }
                    catch (SerialPortException ex) {
                        System.out.println(ex);
                    }
                }
            }
//            else if(event.isCTS()){//If CTS line has changed state
//                if(event.getEventValue() == 1){//If line is ON
//                    System.out.println("CTS - ON");
//                }
//                else {
//                    System.out.println("CTS - OFF");
//                }
//            }
//            else if(event.isDSR()){///If DSR line has changed state
//                if(event.getEventValue() == 1){//If line is ON
//                    System.out.println("DSR - ON");
//                }
//                else {
//                    System.out.println("DSR - OFF");
//                }
//            }
        }
    }
}
