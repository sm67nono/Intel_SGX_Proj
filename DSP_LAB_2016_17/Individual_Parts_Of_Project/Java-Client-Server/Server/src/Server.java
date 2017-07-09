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
				// pass Success-publickey on successfull attestation and
				// pass failed on failure of remote attestation
				byte aesKeyBytes[] = { 'a', 'b', 'c', 'd', 'a', 'b', 'c', 'd',
						'a', 'b', 'c', 'd', 'a', 'b', 'c', 'd' };
				System.out.println(Arrays.toString(aesKeyBytes));
				str = "Success-" + Arrays.toString(aesKeyBytes);
				pw.println(str);
			} else if (str.contains("Expression")) {
				//System.out.println(str);
				//byte aesKeyBytes[] = { 'a', 'b', 'c', 'd', 'a', 'b', 'c', 'd',
					//	'a', 'b', 'c', 'd', 'a', 'b', 'c', 'd' };

				String[] parts = str.split("- ");
				String tt = parts[1].replaceAll("\\[", "")
						.replaceAll("\\]", "");

				String[] nums = tt.split(", ");
				byte expr[] = new byte[nums.length];
				for (int n = 0; n < nums.length; n++) {
					expr[n] = (byte) Integer.parseInt(nums[n]);
				}
				System.out.println("This is Encrypted Epresstion"+ Arrays.toString(expr));
				//EncryptionUtil encryptDecrypt = new EncryptionUtil(aesKeyBytes);
				//String res = encryptDecrypt.decrypt(expr);
				//System.out.println(res +" is the decrypted expression");
				//if (res.equalsIgnoreCase("exit")) {
				//	break;
				//}
				pw.println("Result");
				break;
				
			} else if (str.equalsIgnoreCase("exit")) {
				break;
			}

			// System.out.println("The Encrypted Message is " + str);

			// send this str to the enclave for decrypting and evaluating
			//
			// if (str.equalsIgnoreCase("exit")) {
			// pw.println("exit");
			// // break;
			// } else {
			// str = "Server returns " + str;
			// pw.println(str);
			// }
		}
		pw.close();
		br.close();
		fromClientSocket.close();
		servSocket.close();
	}

}