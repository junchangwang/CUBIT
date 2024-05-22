#
# Created by TianchengHang on 2024/5/8
#
import argparse
import os
import math

if __name__ == "__main__":
    print("=" * 20)
    print("|       EXPERIMENT |")
    print("-" * 20)
    for _exp in ("core", "1B", "earth", "MT", "Para", "Seg", "WPMT", "zipf"):
        print(f"| {_exp.rjust(16)} |")
    print("=" * 20)
    print()

    parser = argparse.ArgumentParser()
    parser.add_argument(
        "-a",
        "--all",
        action="store_true",
        help="execute all experiments, default false",
    )

    parser.add_argument(
        "-e",
        "--exp",
        default="core",
        type=str,
        choices=["core", "1B", "earth", "MT", "Para", "Seg", "WPMT", "zipf"],
        help="which experiment, default 'core'",
    )

    # Command "python3 test.py -e 1B" specifies `./eva-scripts/run_1B.py` on `graphs_0`, `graphs_1`, `graphs_2`
    # Type "python3 test.py --help" to show help message

    args = parser.parse_args()
    if args.all:  # execute all experiments
        exe = input("Execute all experiments, y/n? ")
        if exe != "y" and exe != "Y":
            exit(0)
        for exp in ("core", "1B", "earth", "MT", "Para", "Seg", "WPMT", "zipf"):
            os.system(f"python3 eva-scripts/run_{exp}.py")
            for trial in (0, 1, 2):
                print(
                    "\033[32m "
                    + "=" * 50
                    + f"Experiment {exp} on graphs_{trial}"
                    + "=" * 50
                    + "\033[0m"
                )
                os.system(f"eva-scripts/gen_graphs_{exp}.sh graphs_{trial}")
    else:
        os.system(f"python3 eva-scripts/run_{args.exp}.py")
        for trial in (0, 1, 2):
            print(
                "\033[32m"
                + "=" * 50
                + f"Experiment {args.exp} on graphs_{trial}"
                + "=" * 50
                + "\033[0m"
            )
            os.system(f"eva-scripts/gen_graphs_{args.exp}.sh graphs_{trial}")

    print("Done!")
