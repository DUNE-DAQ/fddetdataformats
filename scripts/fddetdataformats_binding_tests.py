#!/usr/bin/env python3

import subprocess
import sys
from pathlib import Path


def run_script(script_dir: Path, script_name: str) -> int:
    script_path = script_dir / script_name
    if not script_path.exists():
        print(f"MISSING: {script_name}")
        return 1

    print(f"\n=== Running {script_name} ===")
    result = subprocess.run([sys.executable, str(script_path)], check=False)
    return result.returncode


def main() -> int:
    script_dir = Path(__file__).resolve().parent

    scripts = [
        "TDEEth_binding_test.py",
        "DAPHNE_binding_test.py",
        "DAPHNEStream_binding_test.py",
        "DAPHNEEth_binding_test.py",
        "DAPHNEEthStream_binding_test.py",
        "WIBEth_binding_test.py",
    ]

    failures = 0
    for script in scripts:
        failures += 1 if run_script(script_dir, script) != 0 else 0

    print("\n=== Summary ===")
    if failures == 0:
        print("All fddetdataformats binding tests passed")
    else:
        print(f"{failures} binding test script(s) failed")

    return failures


if __name__ == "__main__":
    sys.exit(main())
