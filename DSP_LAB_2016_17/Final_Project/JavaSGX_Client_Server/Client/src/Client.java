import java.io.*;
import java.net.Socket;
import java.util.Arrays;
import java.util.Scanner;

/**
 * @author Clindo Devassy K
 */
public class Client {

	public final static int SOCKET_PORT = 9999; // you may change this
	public final static String SERVER = "127.0.0.1"; // localhost
	static int flag = 0;
	static boolean hasKey = false;


	private static byte[] aesKeyBytes;

	/*This function reads a file and converts to string */
	static String readFile(String fileName) throws IOException {
    	BufferedReader br = new BufferedReader(new FileReader(fileName));
    	try {
        StringBuilder sb = new StringBuilder();
        String line = br.readLine();

        while (line != null) {
            sb.append(line);
            sb.append("\n");
            line = br.readLine();
        }
       	 return sb.toString();
    	} finally {
       	 br.close();
    	}
	}

	public static void main(String argv[]) {

		try {
			String str = "Initialize";
			Socket socketClient = new Socket(SERVER, SOCKET_PORT);
			System.out.println("Client: " + "Connection Established");

			BufferedReader reader = new BufferedReader(new InputStreamReader(
					socketClient.getInputStream()));

			PrintWriter writer = new PrintWriter(
					socketClient.getOutputStream(), true);

			Scanner scanner = new Scanner(System.in);

			writer.println(str);

			while ((str = reader.readLine()) != null) {

				if (str.equalsIgnoreCase("Accepted")) {
					System.out.println("======================Connection Accepted=========================");
					System.out.println("Step-1: Challenge Server Enclave for validity (Remote Attestation)");
					str = "Challenge";
					writer.println(str);
				} 

				else if (str.contains("Success")) {

					System.out.println("===========Remote Attestation Successfull====================");
					System.out.println("Step-2: Key Exchange");
					String[] parts = str.split("-");
					String tt = parts[1].replaceAll("\\[", "").replaceAll("\\]", "");
					String[] nums = tt.split(", ");
					aesKeyBytes = new byte[nums.length];
					for (int n = 0; n < nums.length; n++) {
						aesKeyBytes[n] = (byte) Integer.parseInt(nums[n]); 
						// this
					        // is
						// the
						// public
						// key
						// for encryption and
						// decryption
					}

					//System.out.println("asdas   " + Arrays.toString(aesKeyBytes));
					
					hasKey = true;
					System.out.println("===================Key Exchange Complete================");

				}
				 else if (str.equalsIgnoreCase("Failed")) {
					System.out.println("Remote Attestation Failed");
					System.out.println("Enclave is not secure.. App is exiting");
					break;
				} 

					else if (str.contains("Result")) {

					System.out.println("===========Result Delivered from Server====================");
					System.out.println("Step 4: Decrypt and Show result");
					System.out.println(str);
					String[] parts = str.split("- ");
					
					String tt = parts[1].replaceAll("\\[", "")
						.replaceAll("\\]", "");
					
					String[] nums = tt.split(", ");
					System.out.println("Here");
					byte expr[] = new byte[nums.length];
					for (int n = 0; n < nums.length; n++) {
						expr[n] = (byte) Integer.parseInt(nums[n]);
					}
					EncryptionUtil encryptDecrypt = new EncryptionUtil(aesKeyBytes);
					System.out.println("Actual Result sent by server : "+expr);
					String res = encryptDecrypt.decrypt(expr);
					System.out.println("Decrypted Result : "+res);

					BufferedReader br = new BufferedReader(new InputStreamReader(System.in));
					String inp = "Y";
					System.out.println("Repeat Process (Y/N) ?");
					inp = br.readLine();
					if(inp.equals("Y")){continue;}
					else{
						writer.println("exit");					
						break;}
					
					

				}
				else if (str.equalsIgnoreCase("exit")) {
					System.out.println("Connection Closed by Server");
					break;
				}

				if (hasKey) {
						

					String userInput = "0";
					BufferedReader br = new BufferedReader(new InputStreamReader(System.in));
					
        				
					//Read Expression from file or user input
					
					try

					{
						userInput=readFile("Expression.txt");
					}
					catch(IOException e)
					{
						e.printStackTrace();
					}		
		
					if(userInput.length()<1)
					{
						
						String s = "";
        					System.out.println("Enter an Arithmetic Expression containing only [0-9], [*,+,-] and parentheses :");
					try
					{
						s = br.readLine();
						userInput=s;
					}
					catch(IOException e)
					{
						e.printStackTrace();
					}
			  	 }





					// byte[] encryptedInput = Encrypt.EncData(userInput);
					if (userInput != null) {
						EncryptionUtil encryptDecrypt = new EncryptionUtil(aesKeyBytes);
						byte[] encryptedInput = encryptDecrypt.encrypt(userInput);
						System.out.println(Arrays.toString(encryptedInput));
						//String res= encryptDecrypt.decrypt(encryptedInput);
						//System.out.println(res+" Result");
						str="Expression- "+Arrays.toString(encryptedInput);
						System.out.println("==============Encypt Data with keys and Send to server==============");
						System.out.println("Step 3: Data Sent");
						writer.println(str);
						hasKey=false;
					}
				
					

				}

			

			}

			reader.close();
			writer.close();
			// scanner.close();
			socketClient.close();

		} catch (Exception e) {
			e.printStackTrace();
		}
	}
}
