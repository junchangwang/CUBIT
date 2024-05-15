################
# Tiancheng Hang
################
import argparse
import os

EXPS: list[str] = ["core", "1B", "earth", "MT", "Para", "Seg", "WPMT", "zipf"]

if __name__ == "__main__":
    print("=" * 20)
    print("|       EXPERIMENT |")
    print("-" * 20)
    for _exp in EXPS:
        print(f"| {_exp.rjust(16)} |")
    print("=" * 20)
    print()

    parser = argparse.ArgumentParser()
    parser.add_argument("-a", "--all",
                        action="store_true",
                        help="execute all experiments, default false")

    parser.add_argument("-e", "--exp",
                        default="core",
                        type=str,
                        choices=[EXPS[0], EXPS[1], EXPS[2], EXPS[3], EXPS[4], EXPS[5], EXPS[6], EXPS[7]],
                        help="which experiment, default 'core'")

    parser.add_argument("-t", "--trial",
                        default=0,
                        type=int,
                        choices=[0, 1, 2],
                        help="which trial, default 0")
    # Command `python3 test.py -e 1B -t 1` specifies `eva-scripts/run_1B.py` and `graphs_1`
    # Type `python3 test.py --help` to show help message

    args = parser.parse_args()
    if args.all:  # execute all experiments
        exe = input("Execute all experiments, y/n? ")
        if exe != "y" and exe != "Y":
            exit(0)
        for trial in [0, 1, 2]:
            for exp in EXPS:
                print("\033[32m " + "=" * 20 + f" Experiment {exp} on graphs_{trial} " + "=" * 20 + "\033[0m")
                cmds = (f"python3 eva-scripts/run_{exp}.py",
                        f"eva-scripts/gen_graphs_{exp}.sh graphs_{trial}")

                os.system(cmds[0])
                os.system(cmds[1])
    else:
        print("\033[32m" + "=" * 20 + f" Experiment {args.exp} on graphs_{args.trial} " + "=" * 20 + "\033[0m")
        cmds = (f"python3 eva-scripts/run_{args.exp}.py",
                f"eva-scripts/gen_graphs_{args.exp}.sh graphs_{args.trial}")

        os.system(cmds[0])
        os.system(cmds[1])

    print("Done!")
