#!/usr/bin/env python3
"""
myOS Build System
=================

A professional build script for the myOS kernel project.

Usage:
    python build.py              # Configure + build
    python build.py --clean      # Clean build directory
    python build.py --rebuild    # Clean + configure + build
    python build.py --run        # Build + run in QEMU
    python build.py --help       # Show all options

Author: Mustafa Alotbah
Email: mustafa.alotbah@gmail.com
Copyright: myOS Project
"""

import argparse
import os
import shutil
import subprocess
import sys
from dataclasses import dataclass
from enum import Enum
from pathlib import Path
from typing import Optional, List


# =============================================================================
# CONFIGURATION
# =============================================================================

class Config:
    """Build configuration constants."""
    
    # Directories (relative to project root)
    BUILD_DIR = "build"
    SOURCE_DIR = "myOS"
    OUTPUT_DIR = "bin"
    TOOLCHAIN_DIR = "cmake/toolchains"
    
    # Default toolchain
    DEFAULT_TOOLCHAIN = "native-gcc-12-m32.cmake"
    
    # Build targets
    DEFAULT_TARGET = "myOS"
    
    # QEMU settings
    QEMU_CMD = "qemu-system-i386"
    QEMU_ARGS = ["-cdrom", "bin/myOS.iso", "-serial", "stdio"]


# =============================================================================
# TERMINAL COLORS
# =============================================================================

class Colors:
    """ANSI color codes for terminal output."""
    
    RESET = "\033[0m"
    BOLD = "\033[1m"
    
    # Regular colors
    RED = "\033[31m"
    GREEN = "\033[32m"
    YELLOW = "\033[33m"
    BLUE = "\033[34m"
    MAGENTA = "\033[35m"
    CYAN = "\033[36m"
    WHITE = "\033[37m"
    
    # Bold colors
    BRED = "\033[1;31m"
    BGREEN = "\033[1;32m"
    BYELLOW = "\033[1;33m"
    BBLUE = "\033[1;34m"
    BCYAN = "\033[1;36m"
    
    @classmethod
    def disable(cls):
        """Disable colors (for non-TTY output)."""
        for attr in dir(cls):
            if not attr.startswith('_') and isinstance(getattr(cls, attr), str):
                setattr(cls, attr, "")


# Check if colors should be disabled
if not sys.stdout.isatty():
    Colors.disable()


# =============================================================================
# LOGGING
# =============================================================================

class Log:
    """Logging utilities with colored output."""
    
    @staticmethod
    def info(msg: str):
        print(f"{Colors.BBLUE}[INFO]{Colors.RESET} {msg}")
    
    @staticmethod
    def success(msg: str):
        print(f"{Colors.BGREEN}[OK]{Colors.RESET} {msg}")
    
    @staticmethod
    def warning(msg: str):
        print(f"{Colors.BYELLOW}[WARN]{Colors.RESET} {msg}")
    
    @staticmethod
    def error(msg: str):
        print(f"{Colors.BRED}[ERROR]{Colors.RESET} {msg}", file=sys.stderr)
    
    @staticmethod
    def step(msg: str):
        print(f"\n{Colors.BCYAN}{'='*60}{Colors.RESET}")
        print(f"{Colors.BCYAN}  {msg}{Colors.RESET}")
        print(f"{Colors.BCYAN}{'='*60}{Colors.RESET}\n")
    
    @staticmethod
    def cmd(cmd: str):
        print(f"{Colors.YELLOW}$ {cmd}{Colors.RESET}")


# =============================================================================
# BUILD SYSTEM
# =============================================================================

@dataclass
class BuildOptions:
    """Build options parsed from command line."""
    clean: bool = False
    rebuild: bool = False
    configure_only: bool = False
    build_only: bool = False
    run: bool = False
    debug: bool = False
    release: bool = False
    toolchain: Optional[str] = None
    target: str = Config.DEFAULT_TARGET
    jobs: Optional[int] = None
    verbose: bool = False
    graphviz: bool = False


class BuildSystem:
    """Main build system class."""
    
    def __init__(self, project_root: Path, options: BuildOptions):
        self.root = project_root
        self.options = options
        self.build_dir = self.root / Config.BUILD_DIR
        self.source_dir = self.root / Config.SOURCE_DIR
        self.output_dir = self.root / Config.OUTPUT_DIR
        
        # Resolve toolchain path
        if options.toolchain:
            self.toolchain = self.root / Config.TOOLCHAIN_DIR / options.toolchain
        else:
            self.toolchain = self.root / Config.TOOLCHAIN_DIR / Config.DEFAULT_TOOLCHAIN
    
    def run_command(self, cmd: List[str], cwd: Optional[Path] = None, 
                    check: bool = True) -> subprocess.CompletedProcess:
        """Run a command and return the result."""
        if self.options.verbose:
            Log.cmd(" ".join(cmd))
        
        try:
            result = subprocess.run(
                cmd,
                cwd=cwd or self.root,
                check=check,
                capture_output=not self.options.verbose,
                text=True
            )
            return result
        except subprocess.CalledProcessError as e:
            Log.error(f"Command failed with exit code {e.returncode}")
            if e.stdout:
                print(e.stdout)
            if e.stderr:
                print(e.stderr, file=sys.stderr)
            raise
    
    def clean(self) -> bool:
        """Clean the build directory."""
        Log.step("Cleaning build directory")
        
        if self.build_dir.exists():
            Log.info(f"Removing {self.build_dir}")
            shutil.rmtree(self.build_dir)
        
        if self.output_dir.exists():
            Log.info(f"Removing {self.output_dir}")
            shutil.rmtree(self.output_dir)
        
        Log.success("Clean complete")
        return True
    
    def configure(self) -> bool:
        """Configure the project with CMake."""
        Log.step("Configuring project")
        
        # Create build directory
        self.build_dir.mkdir(parents=True, exist_ok=True)
        
        # Check toolchain exists
        if not self.toolchain.exists():
            Log.error(f"Toolchain not found: {self.toolchain}")
            Log.info(f"Available toolchains in {self.root / Config.TOOLCHAIN_DIR}:")
            toolchain_dir = self.root / Config.TOOLCHAIN_DIR
            if toolchain_dir.exists():
                for f in toolchain_dir.glob("*.cmake"):
                    print(f"  - {f.name}")
            return False
        
        # Build CMake command
        cmake_cmd = [
            "cmake",
            f"-DCMAKE_TOOLCHAIN_FILE={self.toolchain}",
            f"../{Config.SOURCE_DIR}"
        ]
        
        # Add build type
        if self.options.debug:
            cmake_cmd.append("-DCMAKE_BUILD_TYPE=Debug")
        elif self.options.release:
            cmake_cmd.append("-DCMAKE_BUILD_TYPE=Release")
        
        Log.info(f"Toolchain: {self.toolchain.name}")
        Log.info(f"Source: {self.source_dir}")
        Log.info(f"Build: {self.build_dir}")
        
        Log.cmd(" ".join(cmake_cmd))
        
        try:
            result = subprocess.run(
                cmake_cmd,
                cwd=self.build_dir,
                check=True
            )
            Log.success("Configuration complete")
            return True
        except subprocess.CalledProcessError as e:
            Log.error("Configuration failed")
            return False
    
    def build(self) -> bool:
        """Build the project."""
        Log.step(f"Building target: {self.options.target}")
        
        # Check if configured
        if not (self.build_dir / "Makefile").exists():
            Log.warning("Project not configured, running configure first...")
            if not self.configure():
                return False
        
        # Build command
        build_cmd = ["cmake", "--build", ".", "--target", self.options.target]
        
        # Add parallel jobs
        if self.options.jobs:
            build_cmd.extend(["-j", str(self.options.jobs)])
        
        Log.cmd(" ".join(build_cmd))
        
        try:
            result = subprocess.run(
                build_cmd,
                cwd=self.build_dir,
                check=True
            )
            Log.success("Build complete")
            self._print_outputs()
            return True
        except subprocess.CalledProcessError as e:
            Log.error("Build failed")
            return False
    
    def _print_outputs(self):
        """Print generated output files."""
        print(f"\n{Colors.BGREEN}Generated files:{Colors.RESET}")
        
        outputs = [
            ("Kernel", self.output_dir / "kernel.bin"),
            ("ISO", self.output_dir / "myOS.iso"),
            ("SDK Library", self.output_dir / "lib" / "libmyos-sdk.a"),
            ("Libc Library", self.output_dir / "lib" / "libmyos-libc.a"),
        ]
        
        for name, path in outputs:
            if path.exists():
                size = path.stat().st_size
                size_str = self._format_size(size)
                print(f"  {Colors.GREEN}✓{Colors.RESET} {name}: {path} ({size_str})")
            else:
                print(f"  {Colors.RED}✗{Colors.RESET} {name}: {path} (not found)")
    
    @staticmethod
    def _format_size(size: int) -> str:
        """Format file size in human-readable format."""
        for unit in ['B', 'KB', 'MB', 'GB']:
            if size < 1024:
                return f"{size:.1f} {unit}"
            size /= 1024
        return f"{size:.1f} TB"
    
    def run_qemu(self) -> bool:
        """Run the OS in QEMU."""
        Log.step("Running in QEMU")
        
        iso_path = self.output_dir / "myOS.iso"
        if not iso_path.exists():
            Log.error(f"ISO not found: {iso_path}")
            Log.info("Build the project first with: python build.py")
            return False
        
        qemu_cmd = [Config.QEMU_CMD] + Config.QEMU_ARGS
        Log.cmd(" ".join(qemu_cmd))
        Log.info("Press Ctrl+C to stop QEMU")
        
        try:
            subprocess.run(qemu_cmd, cwd=self.root, check=True)
            return True
        except subprocess.CalledProcessError:
            Log.error("QEMU exited with error")
            return False
        except KeyboardInterrupt:
            Log.info("QEMU stopped")
            return True
    
    def generate_graphviz(self) -> bool:
        """Generate dependency graph."""
        Log.step("Generating dependency graph")
        
        if not (self.build_dir / "Makefile").exists():
            Log.warning("Project not configured, running configure first...")
            if not self.configure():
                return False
        
        graphviz_cmd = [
            "cmake",
            "--graphviz=deps.dot",
            f"../{Config.SOURCE_DIR}"
        ]
        
        Log.cmd(" ".join(graphviz_cmd))
        
        try:
            subprocess.run(graphviz_cmd, cwd=self.build_dir, check=True)
            Log.success(f"Generated: {self.build_dir}/deps.dot")
            
            # Try to convert to PNG if dot is available
            if shutil.which("dot"):
                dot_cmd = ["dot", "-Tpng", "deps.dot", "-o", "deps.png"]
                subprocess.run(dot_cmd, cwd=self.build_dir, check=True)
                Log.success(f"Generated: {self.build_dir}/deps.png")
            
            return True
        except subprocess.CalledProcessError:
            Log.error("Failed to generate dependency graph")
            return False
    
    def execute(self) -> int:
        """Execute the build based on options."""
        try:
            # Clean
            if self.options.clean or self.options.rebuild:
                if not self.clean():
                    return 1
                if self.options.clean and not self.options.rebuild:
                    return 0
            
            # Graphviz
            if self.options.graphviz:
                return 0 if self.generate_graphviz() else 1
            
            # Configure only
            if self.options.configure_only:
                return 0 if self.configure() else 1
            
            # Build only (assumes already configured)
            if self.options.build_only:
                return 0 if self.build() else 1
            
            # Default: configure + build
            if not self.configure():
                return 1
            if not self.build():
                return 1
            
            # Run
            if self.options.run:
                if not self.run_qemu():
                    return 1
            
            return 0
            
        except KeyboardInterrupt:
            Log.warning("Build interrupted")
            return 130


# =============================================================================
# CLI
# =============================================================================

def create_parser() -> argparse.ArgumentParser:
    """Create the argument parser."""
    parser = argparse.ArgumentParser(
        description="myOS Build System",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  python build.py                      Configure and build
  python build.py --clean              Clean build directory
  python build.py --rebuild            Clean, configure, and build
  python build.py --run                Build and run in QEMU
  python build.py --configure-only     Only run CMake configure
  python build.py --graphviz           Generate dependency graph
  python build.py -j8                  Build with 8 parallel jobs
  python build.py --toolchain i686-elf-cross.cmake
                                       Use cross-compiler toolchain
        """
    )
    
    # Actions
    actions = parser.add_argument_group("Actions")
    actions.add_argument(
        "--clean", "-c",
        action="store_true",
        help="Clean build and output directories"
    )
    actions.add_argument(
        "--rebuild", "-r",
        action="store_true",
        help="Clean and rebuild from scratch"
    )
    actions.add_argument(
        "--configure-only",
        action="store_true",
        help="Only run CMake configure step"
    )
    actions.add_argument(
        "--build-only",
        action="store_true",
        help="Only run build step (skip configure)"
    )
    actions.add_argument(
        "--run",
        action="store_true",
        help="Run the OS in QEMU after building"
    )
    actions.add_argument(
        "--graphviz",
        action="store_true",
        help="Generate CMake dependency graph"
    )
    
    # Build options
    build_opts = parser.add_argument_group("Build Options")
    build_opts.add_argument(
        "--toolchain", "-t",
        metavar="FILE",
        help=f"Toolchain file name (default: {Config.DEFAULT_TOOLCHAIN})"
    )
    build_opts.add_argument(
        "--target",
        default=Config.DEFAULT_TARGET,
        help=f"Build target (default: {Config.DEFAULT_TARGET})"
    )
    build_opts.add_argument(
        "--jobs", "-j",
        type=int,
        metavar="N",
        help="Number of parallel build jobs"
    )
    build_opts.add_argument(
        "--debug", "-d",
        action="store_true",
        help="Build in debug mode"
    )
    build_opts.add_argument(
        "--release",
        action="store_true",
        help="Build in release mode"
    )
    build_opts.add_argument(
        "--verbose", "-v",
        action="store_true",
        help="Verbose output"
    )
    
    return parser


def find_project_root() -> Path:
    """Find the project root directory."""
    # Start from script location
    script_dir = Path(__file__).resolve().parent
    
    # Check if script is in project root
    if (script_dir / "myOS").is_dir() and (script_dir / "cmake").is_dir():
        return script_dir
    
    # Go up to find project root (contains myOS/ and cmake/)
    for parent in [script_dir.parent, script_dir.parent.parent]:
        if (parent / "myOS").is_dir() and (parent / "cmake").is_dir():
            return parent
    
    # Fall back to current directory
    cwd = Path.cwd()
    if (cwd / "myOS").is_dir():
        return cwd
    
    raise RuntimeError("Could not find project root. Run from project directory.")


def main() -> int:
    """Main entry point."""
    parser = create_parser()
    args = parser.parse_args()
    
    # Print banner
    print(f"""
{Colors.BCYAN}╔══════════════════════════════════════════════════════════╗
║                    myOS Build System                     ║
╚══════════════════════════════════════════════════════════╝{Colors.RESET}
""")
    
    try:
        project_root = find_project_root()
        Log.info(f"Project root: {project_root}")
    except RuntimeError as e:
        Log.error(str(e))
        return 1
    
    # Create options
    options = BuildOptions(
        clean=args.clean,
        rebuild=args.rebuild,
        configure_only=args.configure_only,
        build_only=args.build_only,
        run=args.run,
        debug=args.debug,
        release=args.release,
        toolchain=args.toolchain,
        target=args.target,
        jobs=args.jobs,
        verbose=args.verbose,
        graphviz=args.graphviz,
    )
    
    # Run build
    build_system = BuildSystem(project_root, options)
    return build_system.execute()


if __name__ == "__main__":
    sys.exit(main())

