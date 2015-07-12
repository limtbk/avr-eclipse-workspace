import java.io.File;
import java.util.ArrayList;
import java.util.List;


public class FileScanner {

	/**
	 * @param args
	 */
	public static void main(String[] args) {	
		List<File> fileList = new ArrayList<File>();
		File root = new File("/Users/lim/Projects");
		searchForFiles(root, fileList);
		for(File file : fileList) {
			System.out.print(file.getName());
			System.out.print(" ");
			System.out.println(file.length());
		}
//		System.out.println(fileList);

	}
	
	public static void searchForFiles(File root, List<File> fileList)
	{
	    if(root == null || fileList == null) return;   

	    if(root.isDirectory())
	    {
	        for(File file : root.listFiles())
	        	searchForFiles(file, fileList);
	    }
	    else if(root.isFile())
	    {
	    	fileList.add(root);
	    }
	}

}
