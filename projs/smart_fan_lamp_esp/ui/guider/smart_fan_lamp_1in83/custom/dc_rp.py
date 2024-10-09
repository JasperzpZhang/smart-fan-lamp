import os
import shutil
import magic  # Import the python-magic library

script_dir = os.path.dirname(os.path.abspath(__file__))  # Get the absolute path of the script directory

# Use python-magic to check if the file is binary
def is_binary(file_path):
    mime = magic.Magic(mime=True)  # Create a magic instance to detect MIME type
    mime_type = mime.from_file(file_path)
    
    # If the MIME type starts with 'text', it's a text file, otherwise it's binary
    return not mime_type.startswith('text')

# Try reading the file with different encodings
def read_file_with_fallback(file_path):
    encodings = ['utf-8', 'gbk', 'iso-8859-1', 'utf-16']
    for encoding in encodings:
        try:
            with open(file_path, 'r', encoding=encoding) as file:
                return file.read(), encoding
        except (UnicodeDecodeError, FileNotFoundError, OSError) as e:
            print(f"Error reading file {file_path} with encoding {encoding}: {e}")
            continue
    raise UnicodeDecodeError(f"Unable to read file {file_path}, tried the following encodings: {encodings}")

# Process the files
def process_files():
    print("Starting file processing...\n")
    
    for root, dirs, files in os.walk(script_dir):
        for file in files:
            file_path = os.path.join(root, file)  # Get the full path of the file
            temp_file_path = os.path.join(root, f"{file}.temp")  # Create the temporary file path

            try:
                if is_binary(file_path):  # Process binary files
                    with open(file_path, 'rb') as original_file:
                        content = original_file.read()
                    with open(temp_file_path, 'wb') as temp_file:
                        temp_file.write(content)
                else:  # Process text files
                    content, encoding = read_file_with_fallback(file_path)
                    with open(temp_file_path, 'w', encoding=encoding) as temp_file:
                        temp_file.write(content)

                # Remove the original file
                try:
                    os.remove(file_path)
                except OSError as e:
                    print(f"Unable to delete file {file_path}, possibly a permission issue: {e}")
                    continue

                # Rename the temporary file to the original file name
                try:
                    shutil.move(temp_file_path, file_path)
                except OSError as e:
                    print(f"Unable to move file {temp_file_path} to {file_path}: {e}")
                    continue

                print(f"Completed: {file_path}")

            except Exception as e:
                print(f"Error processing file {file_path}: {e}")
        
        print("-" * 50)

process_files()

input("Press Enter to exit")
