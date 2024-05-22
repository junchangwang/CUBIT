#
# Created by Tiancheng Hang on 2024/5/8
#
import argparse
import os

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
        "-e",
        "--exp",
        nargs="+",
        default=["core"],
        type=str,
        choices=["core", "1B", "earth", "MT", "Para", "Seg", "WPMT", "zipf", "all"],
        help="""
        which experiment, default 'core'. 
        When core is specified, perform 'core' experiments.
        When all is specified, perform ALL experiments.
        """
    )  # Type "python3 test.py --help" to show help message

    args = parser.parse_args()
    if args.exp.__contains__("all"):  # execute ALL experiments
        exe = input("Execute all experiments, y/n? ")
        if exe != "y" and exe != "Y":
            exit(0)
        for exp in ("core", "1B", "earth", "MT", "Para", "Seg", "WPMT", "zipf"):
            # print(f"python3 eva-scripts/run_{exp}.py")
            os.system(f"python3 eva-scripts/run_{exp}.py")
            for trial in (0, 1, 2):
                print(
                    "\033[32m "
                    + "=" * 50
                    + f"Experiment {exp} on graphs_{trial}"
                    + "=" * 50
                    + "\033[0m"
                )
                # print(f"eva-scripts/gen_graphs_{exp}.sh graphs_{trial}")
                os.system(f"eva-scripts/gen_graphs_{exp}.sh graphs_{trial}")
    else:
        for exp in args.exp:
            # print(f"python3 eva-scripts/run_{exp}.py")
            os.system(f"python3 eva-scripts/run_{exp}.py")
            for trial in (0, 1, 2):
                print(
                    "\033[32m"
                    + "=" * 50
                    + f"Experiment {exp} on graphs_{trial}"
                    + "=" * 50
                    + "\033[0m"
                )
                # print(f"eva-scripts/gen_graphs_{exp}.sh graphs_{trial}")
                os.system(f"eva-scripts/gen_graphs_{exp}.sh graphs_{trial}")

    print("Done!")
