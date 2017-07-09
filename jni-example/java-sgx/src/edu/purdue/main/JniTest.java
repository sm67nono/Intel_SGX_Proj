package edu.purdue.main;

import java.io.File;

/**
 * 
 * @author savvas
 *
 */
public class JniTest {

	// directory containing the library to load. Must be absolute
	private static final String LIB_DIR = System.getProperty("user.home")
			+ "/programming/github/jni-sgx/c-sgx/src";

	// name of the library excluding extension
	private static final String LIB_NAME = "libsgx";

	static {

		// generate library extension based on operating system
		String extension = System.getProperty("os.name").equalsIgnoreCase("Linux") ? "so"
				: "jnilib";

		// construct path
		String libpath = LIB_DIR + "/" + LIB_NAME + "." + extension;

		// load library
		File f = new File(libpath);
		if (f.exists())
			System.load(libpath);
		else
			throw new RuntimeException("Library file not found: " + libpath);
	}

	public native void hello();

	public native void helloArguments(String name, int times);

	public native int helloArgumentsReturn(int x, int y);

	public static void main(String[] args) {
		JniTest test = new JniTest();

		test.hello();

		test.helloArguments("John Doe", 2);

		int z = test.helloArgumentsReturn(2, 1);
		System.out.println("z=" + z);
	}
}
