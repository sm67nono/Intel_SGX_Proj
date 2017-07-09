import java.io.*;
import java.security.InvalidAlgorithmParameterException;
import java.security.InvalidKeyException;
import java.security.NoSuchAlgorithmException;
import java.security.NoSuchProviderException;

import javax.crypto.BadPaddingException;
import javax.crypto.Cipher;
import javax.crypto.IllegalBlockSizeException;
import javax.crypto.NoSuchPaddingException;
import javax.crypto.SecretKey;
import java.security.Key;
import javax.crypto.spec.SecretKeySpec;
import javax.crypto.spec.GCMParameterSpec;

/**
 * 
 * DSP_LAB_2016_17: Manna
 * 
 *
 */
public class JavaSGX {

	// Load sgx library
	static {

		
		System.loadLibrary("Enc_Eval");
	
	}

	// Key for encryption and decryption of data

	private byte aesKeyBytes[] = { 'a', 'b', 'c', 'd', 'a', 'b', 'c', 'd', 'a', 'b', 'c', 'd', 'a',
			'b', 'c', 'd' };
	private Key aesKey = new SecretKeySpec(aesKeyBytes, "AES");
	private byte aes_iv[] = { 'a', 'b', 'c', 'd', 'a', 'b', 'c', 'd', 'a', 'b', 'c', 'd' };

	private GCMParameterSpec params = new GCMParameterSpec(128, aes_iv);
	private static final String ALGORITHM = "AES/GCM/NoPadding";
	private Cipher cipherDecrypt = null;

	public JavaSGX() {
		// initialize decryption cipher
		try {
			this.cipherDecrypt = Cipher.getInstance(ALGORITHM, "SunJCE");
			this.cipherDecrypt.init(Cipher.DECRYPT_MODE, aesKey, params);
		} catch (NoSuchAlgorithmException | NoSuchProviderException | NoSuchPaddingException
				| InvalidKeyException | InvalidAlgorithmParameterException e) {
			e.printStackTrace();
			throw new RuntimeException("Unable to initialize AES cipher");
		}
	}

	/**
	 * Native call to initialize the enclave
	 */
	public native void sgx_init();

	/**
	 * Native call to encrypt a value.
	 * 
	 * This method is only used for testing and/or evaluation.
	 * 
	 * @param ptxt
	 *            the valut to be encrypted
	 * 
	 * @returns the encrypted value
	 */
	public native byte[] sgx_enc(String ptxt);

	/**
	 * Native call to decrypt a value
	 * 
	 * This method is only used for testing and/or evaluation.
	 * 
	 * @param ctxt
	 *            the valut to be decrypted
	 * 
	 * @returns the decrypted value
	 */
	public native String sgx_dec(byte[] ctxt);

	/**
	 * Native call to add two integer values. Both these values are in plaintext form.
	 * 
	 * Lack of encryption means SGX offers no security. This method can be used for testing and/or
	 * evaluation
	 * 
	 * @param ptxt1
	 *            plaintext value 1
	 * @param ptxt2
	 *            plaintext value 2
	 * 
	 * @returns the sum of the two values in plaintext form
	 */
	public native int sgx_add(int ptxt1, int ptxt2);

	/**
	 * Native call to add two values. Both input values are encrypted.
	 * 
	 * @param ctxt1
	 *            ciphertxt value 1
	 * @param ctxt2
	 *            ciphertxt value 2
	 * 
	 * @returns the sum of the two values in ciphertext form
	 */
	public native byte[] sgx_add_enc(byte[] ctxt1, byte[] ctxt2);

	/**
	 * Native call to destroy the enclave
	 */
	public native void sgx_destroy();

	/**
	 * Encrypts a given value
	 * 
	 * @param plaintext
	 *            the value to encrypt
	 * 
	 * @return the encrypted value
	 */
	public byte[] encrypt(String plaintext) {

		if (plaintext == null)
			throw new RuntimeException("Empty plaintext in AES encryption");

		byte[] ciphertext = null;

		try {
			// NOTE: encryption object needs to be re-initialized every time since we keep a
			// constant IV for evaluation purposes. Otherwise we get
			// java.security.InvalidAlgorithmParameterException
			Cipher cipherEncrypt = Cipher.getInstance(ALGORITHM, "SunJCE");
			cipherEncrypt.init(Cipher.ENCRYPT_MODE, aesKey, this.params);
			ciphertext = cipherEncrypt.doFinal(plaintext.getBytes("UTF-8"));
		} catch (Exception e) {
			e.printStackTrace();
		}

		return ciphertext;
	}

	/**
	 * Decrypts a given value
	 * 
	 * @param ciphertext
	 *            the value to decrypt
	 * 
	 * @return the decrypted value
	 */
	public String decrypt(byte[] ciphertext) {

		if (ciphertext == null)
			throw new RuntimeException("Empty ciphertext for AES decryption");

		byte[] plaintext = null;
		try {
			plaintext = this.cipherDecrypt.doFinal(ciphertext);
		} catch (IllegalBlockSizeException | BadPaddingException e) {
			e.printStackTrace();
		}

		String plaintextStr = null;
		try {
			plaintextStr = new String(plaintext, "UTF-8");
		} catch (UnsupportedEncodingException e) {
			e.printStackTrace();
		}

		return plaintextStr;
	}

	

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

	static void Evaluate_Value(){
		JavaSGX objSGX = new JavaSGX();
		objSGX.sgx_init();
		//Read Expression from file or user input
		String ptxt1 = "0";
			try
			{
				ptxt1=readFile("Expression.txt");
			}
			catch(IOException e)
			{
				e.printStackTrace();
			}
		
		if(ptxt1.length()<1)
		{
			BufferedReader br = new BufferedReader(new InputStreamReader(System.in));
			String s = "";
        		System.out.println("Enter an Arithmetic Expression containing only [0-9], [*,+,-] and parentheses :");
			try
			{
				s = br.readLine();
				ptxt1=s;
			}
			catch(IOException e)
			{
				e.printStackTrace();
			}
		}
		//Encrypting expression native SGX call
		byte[] ctxt1 = objSGX.sgx_enc(ptxt1.trim());
		String ptxt2 = "Empty";
		BufferedReader br2 = new BufferedReader(new InputStreamReader(System.in));
			String s = "";
        		System.out.println("Enter a Description for the expression :");
			try
			{
				s = br2.readLine();
				ptxt2=s;
			}
			catch(IOException e)
			{
				e.printStackTrace();
			}
		//Encrypting text native SGX call
		
		byte[] ctxt2 = objSGX.sgx_enc(ptxt2.trim());
		
		


		long start = System.nanoTime();
		//Evaluating inside the enclave
		byte[] r = objSGX.sgx_add_enc(ctxt1, ctxt2);
		System.out.println("Add ciphertext time (nano): " + (System.nanoTime() - start));
		//SGX decryption
		String decr = objSGX.sgx_dec(r);
		//Java Decryptionn of the result
		


		//Using SGX Native modules

		System.out.println("Expression without Encrption: " + ptxt1);
		System.out.println("Description without Encrption: " + ptxt2);
		System.out.println("Expression with Native SGX Encrption using Enclave: " + new String(ctxt1));
		System.out.println("Description with Native SGX Encrption using Enclave: " + new String(ctxt2));
		System.out.println("Result: " + new String(r));
		System.out.println("Decrypted Result: " + decr);
		
		//Using Java for encrption and decrption
		//Encrypting expression and text using Java
		byte[] jtxt1 = objSGX.encrypt(ptxt1.trim());
		byte[] jtxt2 = objSGX.encrypt(ptxt2.trim());

		String jdecr= objSGX.decrypt(r);
		System.out.println("Expression without Encrption: " + ptxt1);
		System.out.println("Description without Encrption: " + ptxt2);

		System.out.println("Expression with Java Encrption:" + new String(jtxt1));
		System.out.println("Description with Java Encrption:" + new String(jtxt2));
		System.out.println("Result: " + new String(r));
		System.out.println("Decrypted Result: " + jdecr);




	}

		public String Evaluate_ValueInput(byte[] expression){
		
		JavaSGX objSGX = new JavaSGX();
		objSGX.sgx_init();
		System.out.println("Enclave Initialization Complete");
		//Encrypting expression native SGX call
		byte[] ctxt1 = expression;
		String ptxt2 = "Data";
		byte[] ctxt2 = objSGX.sgx_enc(ptxt2.trim());
		
		


		long start = System.nanoTime();
		//Evaluating inside the enclave
		byte[] r = objSGX.sgx_add_enc(ctxt1, ctxt2);
		System.out.println("Add ciphertext time (nano): " + (System.nanoTime() - start));
		//SGX decryption
		String decr = objSGX.sgx_dec(r);
		//Java Decryptionn of the result
		


		//Using SGX Native modules

		System.out.println("Description without Encrption: " + ptxt2);
		System.out.println("Expression with Native SGX Encrption using Enclave: " + new String(ctxt1));
		System.out.println("Description with Native SGX Encrption using Enclave: " + new String(ctxt2));
		System.out.println("Result: " + new String(r));
		System.out.println("Decrypted Result: " + decr);
		
		//Using Java for encrption and decrption
		//Encrypting expression and text using Java
	
		byte[] jtxt2 = objSGX.encrypt(ptxt2.trim());

		String jdecr= objSGX.decrypt(r);
		System.out.println("Description without Encrption: " + ptxt2);

		System.out.println("Description with Java Encrption:" + new String(jtxt2));
		System.out.println("Result: " + new String(r));
		System.out.println("Decrypted Result: " + jdecr);
	        return jdecr;		


	}

	public static void main(String[] args) {
		
		Evaluate_Value();
	}
}
