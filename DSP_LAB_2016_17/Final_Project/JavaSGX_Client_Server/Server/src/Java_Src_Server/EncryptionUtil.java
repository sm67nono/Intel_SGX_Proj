import java.io.UnsupportedEncodingException;
import java.security.InvalidAlgorithmParameterException;
import java.security.InvalidKeyException;
import java.security.Key;
import java.security.NoSuchAlgorithmException;
import java.security.NoSuchProviderException;

import javax.crypto.BadPaddingException;
import javax.crypto.Cipher;
import javax.crypto.IllegalBlockSizeException;
import javax.crypto.NoSuchPaddingException;
import javax.crypto.spec.GCMParameterSpec;
import javax.crypto.spec.SecretKeySpec;

/**
 * 
 * @author Clindo Devassy K
 * 
 */

public class EncryptionUtil {

	// NB: hardcoded keys, not production software
	// private byte aesKeyBytes[] = { 'a', 'b', 'c', 'd', 'a', 'b', 'c', 'd',
	// 'a',
	// 'b', 'c', 'd', 'a', 'b', 'c', 'd' };
	// private Key aesKey = new SecretKeySpec(aesKeyBytes, "AES");
	// private byte aes_iv[] = { 'a', 'b', 'c', 'd', 'a', 'b', 'c', 'd', 'a',
	// 'b',
	// 'c', 'd' };
	//
	// private GCMParameterSpec params = new GCMParameterSpec(128, aes_iv);

	private Key aesKey;
	private GCMParameterSpec params;
	private static final String ALGORITHM = "AES/GCM/NoPadding";
	private Cipher cipherDecrypt = null;

	public EncryptionUtil(byte aesKeyBytes[]) {

		aesKey = new SecretKeySpec(aesKeyBytes, "AES");
		byte aes_iv[] = { 'a', 'b', 'c', 'd', 'a', 'b', 'c', 'd', 'a', 'b',
				'c', 'd' };

		 params = new GCMParameterSpec(128, aes_iv);
		// initialize decryption cipher
		try {
			this.cipherDecrypt = Cipher.getInstance(ALGORITHM, "SunJCE");
			this.cipherDecrypt.init(Cipher.DECRYPT_MODE, aesKey, params);
		} catch (NoSuchAlgorithmException | NoSuchProviderException
				| NoSuchPaddingException | InvalidKeyException
				| InvalidAlgorithmParameterException e) {
			e.printStackTrace();
			throw new RuntimeException("Unable to initialize AES cipher");
		}
	}

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
			// NOTE: encryption object needs to be re-initialized every time
			// since we keep a
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
}