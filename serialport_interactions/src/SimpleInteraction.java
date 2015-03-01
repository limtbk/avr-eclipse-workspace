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
        int j = 0;
        for(int i = 0; i < portNames.length; i++){
            System.out.println(portNames[i]);
            String portName = portNames[i];
       		SerialPort serialPort = new SerialPort(portName);
       		int mask = SerialPort.MASK_RXCHAR;
    		serialPort.setEventsMask(mask);
//    		serialPort.addEventListener(new SerialPortReader());
			serialPorts[j] = serialPort;
			j++;
        }
        
        System.out.println(serialPorts);

//        try {
//            serialPort.openPort();
//            for (int i = 0; i < 100; i++) {
//                serialPort.setParams(SerialPort.BAUDRATE_115200, 
//                        SerialPort.DATABITS_8,
//                        SerialPort.STOPBITS_1,
//                        SerialPort.PARITY_NONE);
//                serialPort.writeBytes("AT\n\r".getBytes());
//                try {
//    				Thread.sleep(1);
//    			} catch (InterruptedException e) {
//    				e.printStackTrace();
//    			}
//                serialPort.setParams(SerialPort.BAUDRATE_57600, 
//                        SerialPort.DATABITS_8,
//                        SerialPort.STOPBITS_1,
//                        SerialPort.PARITY_NONE);
//                serialPort.writeBytes("AT\n\r".getBytes());
//                try {
//    				Thread.sleep(2);
//    			} catch (InterruptedException e) {
//    				e.printStackTrace();
//    			}
//                serialPort.setParams(SerialPort.BAUDRATE_38400, 
//                        SerialPort.DATABITS_8,
//                        SerialPort.STOPBITS_1,
//                        SerialPort.PARITY_NONE);
//                serialPort.writeBytes("AT\n\r".getBytes());
//                try {
//    				Thread.sleep(4);
//    			} catch (InterruptedException e) {
//    				e.printStackTrace();
//    			}
//                serialPort.setParams(SerialPort.BAUDRATE_19200, 
//                        SerialPort.DATABITS_8,
//                        SerialPort.STOPBITS_1,
//                        SerialPort.PARITY_NONE);
//                serialPort.writeBytes("AT\n\r".getBytes());
//                try {
//    				Thread.sleep(8);
//    			} catch (InterruptedException e) {
//    				e.printStackTrace();
//    			}
//                serialPort.setParams(SerialPort.BAUDRATE_9600, 
//                        SerialPort.DATABITS_8,
//                        SerialPort.STOPBITS_1,
//                        SerialPort.PARITY_NONE);
//                serialPort.writeBytes("AT\n\r".getBytes());
//                try {
//    				Thread.sleep(16);
//    			} catch (InterruptedException e) {
//    				e.printStackTrace();
//    			}
//			}
//            serialPort.closePort();
//        }
//        catch (SerialPortException ex) {
//            System.out.println(ex);
//        }		
		System.out.println("Finish");

	}

}
