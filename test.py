"""Created by Tiancheng Hang on 2024/5/8"""

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
        choices=["core", "1B", "earth", "MT",
                 "Para", "Seg", "WPMT", "zipf", "all"],
        help="""
        which experiment, default 'core'.
        When core is specified, perform 'core' experiments.
        When all is specified, perform ALL experiments.
        """
    )  # Type "python3 test.py --help" to show help message

    args = parser.parse_args()
    if "all" in args.exp:  # execute ALL experiments
        # print("python3 eva-scripts/run_core.py")
        os.system("python3 eva-scripts/run_core.py")
        for trial in ("graphs_0", "graphs_1", "graphs_2"):
            print("\033[32m " + "=" * 50 +
                  f"Experiment core on {trial}" + "=" * 50 + "\033[0m")
            # print(f"eva-scripts/gen_graphs_core.sh {trial}")
            os.system(f"eva-scripts/gen_graphs_core.sh {trial}")
        for exp in ("1B", "earth", "MT", "Para", "Seg", "WPMT", "zipf"):
            # print(f"python3 eva-scripts/run_{exp}.py")
            os.system(f"python3 eva-scripts/run_{exp}.py")
            for trial in os.listdir("./"):
                if not trial.startswith(f"graphs_{exp}"):
                    continue
                print("\033[32m " + "=" * 50 +
                      f"Experiment {exp} on {trial}" + "=" * 50 + "\033[0m")
                # print(f"eva-scripts/gen_graphs_{exp}.sh {trial}")
                os.system(f"eva-scripts/gen_graphs_{exp}.sh {trial}")
    else:
        for exp in args.exp:
            # print(f"python3 eva-scripts/run_{exp}.py")
            os.system(f"python3 eva-scripts/run_{exp}.py")
            if exp == "core":
                for trial in ("graphs_0", "graphs_1", "graphs_2"):
                    print("\033[32m " + "=" * 50 +
                          f"Experiment core on {trial}" + "=" * 50 + "\033[0m")
                    # print(f"eva-scripts/gen_graphs_core.sh {trial}")
                    os.system(f"eva-scripts/gen_graphs_core.sh {trial}")
                continue
            for trial in os.listdir("./"):
                if not trial.startswith(f"graphs_{exp}"):
                    continue
                print("\033[32m " + "=" * 50 +
                      f"Experiment {exp} on {trial}" + "=" * 50 + "\033[0m")
                # print(f"eva-scripts/gen_graphs_{exp}.sh {trial}")
                os.system(f"eva-scripts/gen_graphs_{exp}.sh {trial}")

    print("Done!")
