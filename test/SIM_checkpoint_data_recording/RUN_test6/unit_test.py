import trick

def main():
    
    exec(open("Modified_data/foo.dr").read())

    # trick.checkpoint(7.0)
    trick.add_read(5.0, 'trick.load_checkpoint("RUN_test6/chkpnt_7.000000")') # contains data recording, starts at t=7

    trick.stop(10.0)

if __name__ == "__main__":
    main()