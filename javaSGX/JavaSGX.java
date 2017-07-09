import java.io.File;
import java.io.UnsupportedEncodingException;
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
 * @author savvas@purdue.edu
 *
 */
public class JavaSGX {

	// Load sgx library
	static {

		String libpath = System.getenv("LIBENCLAVE_PATH");

		if (libpath == null)
			throw new RuntimeException("LIBENCLAVE_PATH not set!");

		// load library
		File f = new File(libpath);
		if (f.exists()) {
			// check if the file exists and load it.
			System.load(libpath);
		} else
			throw new RuntimeException("Library file not found: " + libpath);
	}

	// NB: hardcoded keys, not production software
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

	/**
	 * Test encryption and decryption in java only
	 */
	static void testJavaEncDec() {
		JavaSGX test = new JavaSGX();

		String ptxt = "1234";
		byte[] ctxt = test.encrypt(ptxt);
		String decr = test.decrypt(ctxt);
		System.out.println("Plaintext: " + ptxt);
		System.out.println("Ciphertext: " + new String(ctxt));
		System.out.println("Decrypted: " + decr);

		ptxt = "abc";
		ctxt = test.encrypt(ptxt);
		decr = test.decrypt(ctxt);
		System.out.println("Plaintext: " + ptxt);
		System.out.println("Ciphertext: " + new String(ctxt));
		System.out.println("Decrypted: " + decr);
	}

	static void testAdd() {
		JavaSGX test = new JavaSGX();

		long start = System.nanoTime();
		test.sgx_init();
		System.out.println("Init enclave time (nano): " + (System.nanoTime() - start));

		start = System.nanoTime();
		int r = test.sgx_add(123, 222);
		System.out.println("Add plaintext time (nano): " + (System.nanoTime() - start));

		System.out.println("Result: " + r);

		start = System.nanoTime();
		test.sgx_destroy();
		System.out.println("Destroy enclave time (nano): " + (System.nanoTime() - start));
	}

	static void testEnc() {
		JavaSGX test = new JavaSGX();
		test.sgx_init();

		String ptxt = "123";

		long start = System.nanoTime();
		byte[] r = test.sgx_enc(ptxt);
		System.out.println("Encrypt time (nano): " + (System.nanoTime() - start));
		String decr = test.decrypt(r);

		System.out.println("Plaintext: " + ptxt);
		System.out.println("Result: " + new String(r));
		System.out.println("Decrypted: " + decr);
	}

	static void testDec() {
		JavaSGX test = new JavaSGX();
		test.sgx_init();

		String ptxt = "123";

		byte[] ctxt = test.encrypt(ptxt);

		long start = System.nanoTime();
		String r = test.sgx_dec(ctxt);
		System.out.println("Decrypt time (nano): " + (System.nanoTime() - start));

		System.out.println("Plaintext: " + ptxt);
		System.out.println("Ciphertext: " + new String(ctxt));
		System.out.println("Decrypted: " + r);
	}

	static void testAddEnc() {
		JavaSGX test = new JavaSGX();
		test.sgx_init();

		String ptxt1 = "123";
		byte[] ctxt1 = test.encrypt(ptxt1);
		String ptxt2 = "222";
		byte[] ctxt2 = test.encrypt(ptxt2);

		long start = System.nanoTime();
		byte[] r = test.sgx_add_enc(ctxt1, ctxt2);
		System.out.println("Add ciphertext time (nano): " + (System.nanoTime() - start));

		String decr = test.decrypt(r);

		System.out.println("Plaintext1: " + ptxt1);
		System.out.println("Plaintext2: " + ptxt2);
		System.out.println("Ciphertext1: " + new String(ctxt1));
		System.out.println("Ciphertext2: " + new String(ctxt2));
		System.out.println("Result: " + new String(r));
		System.out.println("Decrypted: " + decr);

	}

	public static void main(String[] args) {
		testJavaEncDec();
		testAdd();
		testEnc();
		testDec();
		testAddEnc();
	}
}
