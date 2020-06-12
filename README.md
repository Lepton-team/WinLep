# WinLep

Is a **Win**dows only CLI application/tool and a image format based on the [**Lep**ton](https://github.com/dropbox/lepton) image format for "lossless JPEG compression by an average of 22%". This can be mostly used to archive large photo collections.

WinLep was created to make use of the awesome Lepton tool/format for a Windows user which at least knows how to use the CMD, rewarding him with lots of free disk space.
(GUI version probably coming soon)

When you install WinLep, technically you install more free disk space. 

WinLep supports the conversion of all of the following file extensions:
- `.jpg, .jpeg, .jpe, .jif, .jfif, .jfi`

In addition to Lepton, WinLep has a [thumbnail image preview\*](https://github.com/Lepton-team/WinLep_DLL) to make it easier to navigate through converted photos, versioning system for future backward compatibility and a easy-to-use, extended command line interface.

\* Disclaimer: In order to make the thumbnails display correctly, you need to have registered [this DLL](https://github.com/Lepton-team/WinLep_DLL). (For now).


# Upcoming features
There are many features on our todo list, here are some quality-of-life ones.
- Easy to use Windows installer
- WinLep image viewer
- Windows shell extension to easily convert JPEGs to WinLep and vice versa. Meaning whenever you right-click on a JPEG image an option will pop up in the menu to convert it to WinLep, and when you right-click a WinLep image an option will show up to convert it to JPEG.
- WinLep property handler DLL
- Make thumbnail size variable

# How to use WinLep 
\*Note: This manual is more or less the same as WinLep's help page

- **NAME**
    - **WinLep** - Convert JPEG images to .wlep and vice versa 
- **SYNOPSIS**
    - WinLep <input_file> [output_file] [options]
    - WinLep [options] [-d] [-D] <directory> [output_directory]
- **DESCRIPTION**
    - WinLep is a windows exclusive command line application used for converting
        JPEG images to .wlep format and vice versa. The biggest benefit of .wlep
        format is the file size. In average it's 22% smaller than original JPEG.
    - The compression/decompression is lossless and the original file is preserved bit-for-bit perfeclty
- **OPTIONS**
```   
        -h --help   
                - Show this menu
        -w      - (default) Converts desired file/directory from JPEG to .wlep
        -r      - Removes original images
        -j      - Converts desired file/directory from .wlep to .jpg
        -J      - Converts desired file/directory from .wlep to .jpg
                and REMOVES all original .wlep images. Same as -jr
        -W      - Converts desired file/directory from .jpg to .wlep
                and REMOVES all original JPEG images. Same as -wr
        -v --verbose
                - Outputs more information to the console
        -d <directory> [output_directory]
                - Converts all JPEG images in the given directory
                to [output_directory]. Original filenames will be used, with the .wlep extension
                If no [output_directory] is provided, the files will be outputted to [directory].
                If [output_directory] is provided and it doesn't exist, it's created.
        -D <directory> [output_directory]
                - Converts all JPEG images in the given directory and all of its subdirectories to [output_directory].
                Original filenames will be used with the added .wlep extension.
                If no [output_directory] is provided, the files will be outputted to [directory]
                If [output_directory] is provided and it doesn't exist, it's created
                along with all other subdirectories.
                All original subdirectory names are preserved and created in the same structure
```                
- **EXAMPLES**
    -   ` WinLep test.jpg` --> Converts test.jpg and saves it as test.wlep
    -   `WinLep test.wlep picture` --> Converts test.wlep and saves it as picture.jpg
    -   `WinLep test.wlep pictures\` --> Converts test.wlep and saves it into pictures\test.jpg
    -   `WinLep -w test.jpg out.wlep` --> Converts test.jpg and saves it as out.wlep
    -   `WinLep -jd .` --> Converts all the .wlep images in the current directory to .jpg
    -   `WinLep -d . wlep_images` --> Converts all the JPEG images in the current directory
                         and saves them into wlep_images folder.
    -   `WinLep -wD .`--> Converts all JPEG images in the current directory and all of its subdirectories.
                         Converted files are created in the same subfolders as the original ones.
    -   ` WinLep -Dj . pics -v`--> Converts all .wlep images in the current folder to .jpg with additinal output and writes them in the subfolder 'pics' creating the original subfolder structure.

- **BUGS** 
    - Let us know.

- **AUTHOR**
    - Marek Okša, xoksamarek@gmail.com
