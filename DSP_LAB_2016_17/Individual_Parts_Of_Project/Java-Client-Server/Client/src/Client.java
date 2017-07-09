import java.io.BufferedReader;
import java.io.InputStreamReader;
import java.io.PrintWriter;
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

	// private static byte aesKeyBytes[] = { 'a', 'b', 'c', 'd', 'a', 'b', 'c',
	// 'd', 'a', 'b', 'c', 'd', 'a', 'b', 'c', 'd' };
	// static EncryptionUtil encryptDecrypt = new EncryptionUtil(aesKeyBytes);

	private static byte[] aesKeyBytes;

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
					System.out.println(str);
					str = "Challenge";
					writer.println(str);
				} else if (str.contains("Success")) {

					System.out.println("Success");
					String[] parts = str.split("-");
					String tt = parts[1].replaceAll("\\[", "").replaceAll(
							"\\]", "");
					System.out.println(tt);

					String[] nums = tt.split(", ");
					aesKeyBytes = new byte[nums.length];
					for (int n = 0; n < nums.length; n++) {
						aesKeyBytes[n] = (byte) Integer.parseInt(nums[n]); // this
																			// is
						// the
						// public
						// key
						// for encryption and
						// decryption
					}

					System.out.println("asdas   " + Arrays.toString(aesKeyBytes));

					hasKey = true;

				} else if (str.equalsIgnoreCase("Failed")) {
					System.out.println(str);
					System.out
							.println("Enclave is not secure.. App is exiting");
					break;
				} else if (str.equalsIgnoreCase("exit")) {
					System.out.println("Connection Closed by Server");
					break;
				}

				if (hasKey) {
					System.out.println("enter a expression :");
					String userInput = scanner.nextLine();

					// byte[] encryptedInput = Encrypt.EncData(userInput);
					if (userInput != null) {
						EncryptionUtil encryptDecrypt = new EncryptionUtil(aesKeyBytes);
						byte[] encryptedInput = encryptDecrypt
								.encrypt(userInput);
						System.out.println(Arrays.toString(encryptedInput));
						String res= encryptDecrypt.decrypt(encryptedInput);
						System.out.println(res+" hooi");
						str="Expression- "+Arrays.toString(encryptedInput);
						writer.println(str);
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