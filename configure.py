#!/usr/bin/env python3
"""
myOS Template Configuration Script

This script automates the renaming of the OS template to your custom project name.
It handles:
  - Updating namespaces (myOS -> YourOS)
  - Updating include paths
  - Updating CMake configurations
  - Renaming directories
  - Updating comments and documentation

Usage:
    python configure.py --name PalmyraOS
    python configure.py --name bestOS --author "John Doe"

Author: Mustafa Alotbah
Contact: mustafa.alotbah@gmail.com
"""

import os
import re
import argparse
from pathlib import Path


def parse_arguments():
    """Parse command-line arguments."""
    parser = argparse.ArgumentParser(
        description="Configure the OS template with your project name.",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
    python configure.py --name PalmyraOS
    python configure.py --name MyKernel --author "Jane Doe"
        """
    )
    parser.add_argument(
        '--name', '-n',
        required=True,
        help="The new OS/project name (e.g., PalmyraOS, MyKernel)"
    )
    parser.add_argument(
        '--author', '-a',
        default=None,
        help="Author name for copyright notices (optional)"
    )
    parser.add_argument(
        '--dry-run', '-d',
        action='store_true',
        help="Show what would be changed without making changes"
    )
    return parser.parse_args()


def find_files(base_path, extensions=None, exclude_dirs=None):
    """
    Find all files with given extensions, excluding certain directories.
    
    Args:
        base_path: Root directory to search
        extensions: List of file extensions to include (e.g., ['.cpp', '.h'])
        exclude_dirs: List of directory names to exclude
    
    Returns:
        List of Path objects
    """
    if extensions is None:
        extensions = ['.cpp', '.h', '.hpp', '.c', '.asm', '.cmake', '.txt', '.cfg', '.ld', '.py', '.md']
    if exclude_dirs is None:
        exclude_dirs = ['build', 'bin', '.git', '__pycache__', 'CMakeFiles', '.vscode']
    
    files = []
    for root, dirs, filenames in os.walk(base_path):
        # Remove excluded directories from traversal
        dirs[:] = [d for d in dirs if d not in exclude_dirs]
        
        for filename in filenames:
            if any(filename.endswith(ext) for ext in extensions):
                files.append(Path(root) / filename)
    
    return files


def replace_in_file(file_path, replacements, dry_run=False):
    """
    Replace text in a file.
    
    Args:
        file_path: Path to the file
        replacements: Dict of {old_text: new_text}
        dry_run: If True, don't actually modify the file
    
    Returns:
        True if changes were made, False otherwise
    """
    try:
        with open(file_path, 'r', encoding='utf-8', errors='ignore') as f:
            content = f.read()
    except Exception as e:
        print(f"  [WARN] Could not read {file_path}: {e}")
        return False
    
    original_content = content
    
    for old, new in replacements.items():
        content = content.replace(old, new)
    
    if content != original_content:
        if not dry_run:
            try:
                with open(file_path, 'w', encoding='utf-8', newline='\n') as f:
                    f.write(content)
            except Exception as e:
                print(f"  [ERROR] Could not write {file_path}: {e}")
                return False
        return True
    
    return False


def rename_path(old_path, new_path, dry_run=False):
    """
    Rename a file or directory.
    
    Args:
        old_path: Current path
        new_path: New path
        dry_run: If True, don't actually rename
    
    Returns:
        True if renamed, False otherwise
    """
    if not os.path.exists(old_path):
        return False
    
    if os.path.exists(new_path):
        print(f"  [WARN] Target already exists: {new_path}")
        return False
    
    if not dry_run:
        os.rename(old_path, new_path)
    
    return True


def configure_project(base_path, new_name, author=None, dry_run=False):
    """
    Configure the OS template with a new project name.
    
    Args:
        base_path: Root directory of the project
        new_name: New OS/project name
        author: Optional author name
        dry_run: If True, show changes without applying
    """
    old_name = "myOS"
    old_name_lower = "myos"
    old_name_upper = "MYOS"
    
    new_name_lower = new_name.lower()
    new_name_upper = new_name.upper()
    
    print(f"\n{'='*60}")
    print(f"  Configuring OS Template")
    print(f"{'='*60}")
    print(f"  Old name: {old_name}")
    print(f"  New name: {new_name}")
    if author:
        print(f"  Author:   {author}")
    print(f"  Dry run:  {dry_run}")
    print(f"{'='*60}\n")
    
    # Define all replacements
    replacements = {
        # Namespaces and identifiers
        f'namespace {old_name}': f'namespace {new_name}',
        f'{old_name}::': f'{new_name}::',
        f'<{old_name}/': f'<{new_name}/',
        f'"{old_name}/': f'"{new_name}/',
        
        # CMake project and target names
        f'project({old_name}': f'project({new_name}',
        f'--target {old_name}': f'--target {new_name}',
        f'create_bootable_iso({old_name}': f'create_bootable_iso({new_name}',
        f'add_custom_target({old_name}': f'add_custom_target({new_name}',
        f'add_dependencies({old_name}': f'add_dependencies({new_name}',
        f"ISO target '{old_name}'": f"ISO target '{new_name}'",
        f'=== {old_name} Build': f'=== {new_name} Build',
        f'../{old_name}': f'../{new_name}',
        
        # CMake library names
        f'myos-sdk': f'{new_name_lower}-sdk',
        f'myos-libc': f'{new_name_lower}-libc',
        f'lib{old_name_lower}': f'lib{new_name_lower}',
        
        # Macros and defines
        f'{old_name_upper}_': f'{new_name_upper}_',
        f'_{old_name_upper}': f'_{new_name_upper}',
        
        # Comments and documentation
        f'# {old_name} CMakeLists': f'# {new_name} CMakeLists',
        f'# {old_name} kernel': f'# {new_name} kernel',
        f'the {old_name} SDK': f'the {new_name} SDK',
        f'@copyright {old_name}': f'@copyright {new_name}',
        f'{old_name} Project': f'{new_name} Project',
        f'{old_name} Kernel': f'{new_name} Kernel',
        f'{old_name} SDK': f'{new_name} SDK',
        f'{old_name} kernel': f'{new_name} kernel',
        f'{old_name} Build System': f'{new_name} Build System',
        f'Hello from {old_name}': f'Hello from {new_name}',
        
        # File paths in includes
        f'myOS/': f'{new_name}/',
        
        # GRUB config
        f'menuentry "{old_name}"': f'menuentry "{new_name}"',
        f'/{old_name}.iso': f'/{new_name}.iso',
        f'/boot/{old_name}': f'/boot/{new_name}',
        
        # ISO and binary names
        f'{old_name}.iso': f'{new_name}.iso',
        
        # Quoted names (various contexts)
        f'"{old_name}"': f'"{new_name}"',
        f"'{old_name}'": f"'{new_name}'",
        
        # Directory references
        f'({old_name})': f'({new_name})',
        f'/{old_name}': f'/{new_name}',
    }
    
    # Add author replacement if provided
    if author:
        replacements['myOS Project'] = f'{new_name} Project'
        replacements['Mustafa Alotbah'] = author
    
    # Find all files to process
    print("[1/4] Finding files to process...")
    files = find_files(base_path)
    print(f"      Found {len(files)} files\n")
    
    # Process file contents
    print("[2/4] Updating file contents...")
    modified_count = 0
    for file_path in files:
        if replace_in_file(file_path, replacements, dry_run):
            rel_path = file_path.relative_to(base_path)
            print(f"      {'[DRY] ' if dry_run else ''}Modified: {rel_path}")
            modified_count += 1
    print(f"      {modified_count} files {'would be ' if dry_run else ''}modified\n")
    
    # Rename directories
    print("[3/4] Renaming directories...")
    
    # Directories to rename (in order - deepest first)
    dir_renames = [
        # SDK include directory
        (f'{old_name}/sdk/include/{old_name}', f'{old_name}/sdk/include/{new_name}'),
        # Kernel include directories
        (f'{old_name}/kernel/include/{old_name}', f'{old_name}/kernel/include/{new_name}'),
        # Arch include directories
        (f'{old_name}/arch/x86/include/{old_name}', f'{old_name}/arch/x86/include/{new_name}'),
        # Main directory (last!)
        (f'{old_name}', f'{new_name}'),
    ]
    
    for old_dir, new_dir in dir_renames:
        old_path = base_path / old_dir
        new_path = base_path / new_dir
        if rename_path(old_path, new_path, dry_run):
            print(f"      {'[DRY] ' if dry_run else ''}Renamed: {old_dir} -> {new_dir}")
    
    # Update build.py paths
    print("\n[4/4] Updating build script...")
    build_script = base_path / 'build.py'
    if build_script.exists():
        build_replacements = {
            # String literals
            f'"{old_name}"': f'"{new_name}"',
            f"'{old_name}'": f"'{new_name}'",
            # Paths
            f'/{old_name}/': f'/{new_name}/',
            f'/ "{old_name}"': f'/ "{new_name}"',
            # ISO and filenames
            f'{old_name}.iso': f'{new_name}.iso',
            f'bin/{old_name}': f'bin/{new_name}',
            # Documentation
            f'{old_name} Build System': f'{new_name} Build System',
            f'{old_name} kernel': f'{new_name} kernel',
            f'{old_name} Project': f'{new_name} Project',
            # Directory checks
            f'("{old_name}")': f'("{new_name}")',
        }
        if replace_in_file(build_script, build_replacements, dry_run):
            print(f"      {'[DRY] ' if dry_run else ''}Modified: build.py")
    
    # Summary
    print(f"\n{'='*60}")
    if dry_run:
        print("  DRY RUN COMPLETE - No changes were made")
        print("  Run without --dry-run to apply changes")
    else:
        print("  CONFIGURATION COMPLETE!")
        print(f"\n  Your OS is now named: {new_name}")
        print(f"\n  Next steps:")
        print(f"    1. cd {new_name}")
        print(f"    2. python ../build.py --rebuild")
        print(f"    3. python ../build.py --run")
    print(f"{'='*60}\n")


def main():
    """Main entry point."""
    args = parse_arguments()
    
    # Validate name
    if not args.name.isidentifier():
        print(f"Error: '{args.name}' is not a valid identifier.")
        print("Name must start with a letter and contain only letters, numbers, and underscores.")
        return 1
    
    base_path = Path(os.getcwd())
    
    # Check if we're in the right directory
    if not (base_path / 'myOS').exists() and not (base_path / 'build.py').exists():
        print("Error: This script must be run from the OS template root directory.")
        print("Expected to find 'myOS' directory or 'build.py' in current directory.")
        return 1
    
    configure_project(base_path, args.name, args.author, args.dry_run)
    return 0


if __name__ == '__main__':
    exit(main())

