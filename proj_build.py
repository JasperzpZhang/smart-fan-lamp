import os

# Assuming this script is located in the same directory as your current files
current_directory = os.path.dirname(os.path.abspath(__file__))

def create_folders_if_not_exist(folders):
    for folder in folders:
        if not os.path.exists(folder):
            os.makedirs(folder)
            print(f"Created folder: {folder}")
        else:
            print(f"Folder already exists: {folder}")

def create_project_structure(root_folder_name):
    # Current working directory
    current_directory = os.getcwd()
    new_root_folder = os.path.join(current_directory, root_folder_name)

    # Define folder structure
    folders_to_create = [
        new_root_folder,
        os.path.join(new_root_folder, 'bin'),

        os.path.join(new_root_folder, 'prj', 'cubemx'),

        os.path.join(new_root_folder, 'prj', 'src', 'arm', 'app'),
        os.path.join(new_root_folder, 'prj', 'src', 'arm', 'app', 'svc'),

        os.path.join(new_root_folder, 'prj', 'src', 'arm', 'bl'),
        
        os.path.join(new_root_folder, 'prj', 'src', 'arm', 'lib'),
        os.path.join(new_root_folder, 'prj', 'src', 'arm', 'drv', 'peri'),

        os.path.join(new_root_folder, 'prj', 'src', 'c51', 'app'),
        os.path.join(new_root_folder, 'prj', 'src', 'c51', 'svc'),
        os.path.join(new_root_folder, 'prj', 'src', 'c51', 'lib'),
        os.path.join(new_root_folder, 'prj', 'src', 'c51', 'drv', 'peri'),

        os.path.join(new_root_folder, 'tool'),
        os.path.join(new_root_folder, 'tool', 'upd'),
        os.path.join(new_root_folder, 'tool', 'burn'),
        os.path.join(new_root_folder, 'tool', 'sct'),
        
        os.path.join(new_root_folder, 'doc'),
    ]

    # Create folders if they don't exist
    create_folders_if_not_exist(folders_to_create)

# Example usage
create_project_structure('project_name')
