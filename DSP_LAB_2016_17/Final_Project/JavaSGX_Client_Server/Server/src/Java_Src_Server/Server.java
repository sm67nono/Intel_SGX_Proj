/**
 * @author Clindo Devassy K
 */

import java.io.BufferedReader;
import java.io.InputStreamReader;
import java.io.PrintWriter;
import java.net.ServerSocket;
import java.net.Socket;
import java.util.Arrays;


public class Server {
	private	static byte aesKeyBytes[] = { 'a', 'b', 'c', 'd', 'a', 'b', 'c', 'd',
						'a', 'b', 'c', 'd', 'a', 'b', 'c', 'd' };

	public static void main(String argv[]) throws Exception {

		int PORT = 9999;
		String str;
		// System.out.println(" Server is Running  at port " + PORT);
		ServerSocket servSocket = new ServerSocket(PORT);

		System.out.println("Waiting for a connection on " + PORT);

		Socket fromClientSocket = servSocket.accept();
		PrintWriter pw = new PrintWriter(fromClientSocket.getOutputStream(),
				true);

		BufferedReader br = new BufferedReader(new InputStreamReader(
				fromClientSocket.getInputStream()));

		while ((str = br.readLine()) != null) {
			// System.out.println(str);
			if (str.equalsIgnoreCase("Initialize")) {
				System.out.println("Connection Initialized");
				str = "Accepted";
				pw.println(str);

			} else if (str.equalsIgnoreCase("Challenge")) {
				System.out.println("Client Challenged the server");

				// do the remote attestation
				Java_Main javaRA = new Java_Main();
				String result_RA = javaRA.call_RA_SIM();
				// pass Success-publickey on successfull attestation and
				// pass failed on failure of remote attestation
				if(result_RA.contains("Success")){
				System.out.println(Arrays.toString(aesKeyBytes));
				str = "Success-" + Arrays.toString(aesKeyBytes);
				pw.println(str);	
				}
				else
				{	pw.println("Failed");
					System.out.println("Server Exiting RA Failed");
					break;	}
				
			} else if (str.contains("Expression")) {

				String[] parts = str.split("- ");
				String tt = parts[1].replaceAll("\\[", "")
						.replaceAll("\\]", "");

				String[] nums = tt.split(", ");
				byte expr[] = new byte[nums.length];
				for (int n = 0; n < nums.length; n++) {
					expr[n] = (byte) Integer.parseInt(nums[n]);
				}
				JavaSGX sgxEnclave = new JavaSGX();
				System.out.println("This is Encrypted Epresstion"+ Arrays.toString(expr));
				EncryptionUtil encryptDecrypt = new EncryptionUtil(aesKeyBytes);
				String res = encryptDecrypt.decrypt(expr);
				byte[] exprEncrypt = sgxEnclave.encrypt(res.trim());
				System.out.println("Calling Evaluator inside Enclave");
				String finalResult = sgxEnclave.Evaluate_ValueInput(exprEncrypt);
				System.out.println("Result before Encryption: "+finalResult);
				byte[] encryptedInfo = encryptDecrypt.encrypt(finalResult);
				System.out.println("Result is encrypted and sent to Client: "+encryptedInfo.toString());						
				pw.println("Result- "+Arrays.toString(encryptedInfo));
				//break;
				
			} else if (str.equalsIgnoreCase("exit")) {
				System.out.println("Stopping Server.");	
				break;
			}
			
				
			
			

			
		}
		pw.close();
		br.close();
		fromClientSocket.close();
		servSocket.close();
	}

}
