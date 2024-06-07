package main

import (
	"fmt"
	"os"
)

func main() {
	fName := "/mnt/ext4/hello.txt"
	file, err := os.OpenFile(fName, os.O_TRUNC|os.O_WRONLY|os.O_CREATE, 0664)
	if err != nil {
		fmt.Fprintf(os.Stderr, "failed to OpenFile(%s), err: %s\n",
			fName, err.Error())
	}
	defer file.Close()

	_, err = file.WriteString("hello world\n")
	if err != nil {
		fmt.Fprintf(os.Stderr, "failed to WriteString(), err: %s\n",
			err.Error())
	}

	// file.Sync()
}
