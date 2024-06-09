package main

import (
	"fmt"
	"os"
)

func main() {
	fh, err := os.OpenFile("/tmp/hello.txt", os.O_RDWR|os.O_APPEND|os.O_CREATE, 0664)
	if err != nil {
		fmt.Fprintf(os.Stderr, "open file error: %s\n", err.Error())
		return
	}
	defer fh.Close()

	fh.WriteString("hello")

	fh.Sync()
}
