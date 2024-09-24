# -*- coding: gbk -*-

''' Imports '''
import os
import sys
import shutil
import datetime

# boot app checkout (app : 1; boot : 2)
use = 1

def ensure_directory_exists(file_path):
    if not os.path.exists(file_path):
            os.makedirs(file_path)

def main():
    ''' Main - Entry point '''
    try:
        # ��ȡ��ǰ����Ŀ¼
        current_dir = os.getcwd()

        # ����·��
        if use == 1 :
            src_name = os.path.normpath(os.path.join(current_dir, 'app.bin'))
            bin_dir = os.path.normpath(os.path.join(current_dir, '..', '..', '..', '..', '..', 'bin', 'v0.1', 'arm_app'))
        elif use == 2 :
            src_name = os.path.normpath(os.path.join(current_dir, 'boot.bin'))
            bin_dir = os.path.normpath(os.path.join(current_dir, '..', '..', '..', '..', '..', 'bin', 'v0.1', 'arm_bl'))

        dst_name1 = os.path.normpath(os.path.join(current_dir, '..', '..', '..', '..', '..', 'tools', 'burn', 'bin'))
        dst_name2 = os.path.normpath(os.path.join(current_dir, '..', '..', '..', '..', '..', 'tools', 'boot', 'uart_boot_tool'))

        # ȷ��Ŀ��Ŀ¼����
        ensure_directory_exists(bin_dir)
        ensure_directory_exists(dst_name1)
        ensure_directory_exists(dst_name2)

        # ���ɴ�ʱ������ļ���
        if use == 1 :
            image_name = "arm_app"
        elif use == 2:
            image_name = "arm_boot"
        time_stamp = datetime.datetime.now().strftime('%Y%m%d-%H%M%S')
        dst_name_time = os.path.join(bin_dir, f"{image_name}_{time_stamp}.bin")

        # ������Ϊapp.bin / boot.bin
        if use == 1 :
            dst_name1_burn = os.path.join(dst_name1, "app.bin")
            dst_name2_boot = os.path.join(dst_name2, "app.bin")
        elif use == 2:
            dst_name1_burn = os.path.join(dst_name1, "boot.bin")
            dst_name2_boot = os.path.join(dst_name2, "boot.bin")

        # ���Դ�ļ��Ƿ����
        if not os.path.exists(src_name):
            sys.exit(1)

        # �����ļ�
        shutil.copy(src_name, dst_name_time)
        shutil.copy(src_name, dst_name1_burn)
        if use == 1 :
            shutil.copy(src_name, dst_name2_boot)

    except Exception as e:
        sys.exit(1)

if __name__ == '__main__':
    main()
