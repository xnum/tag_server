package main

import (
	"bufio"
	"fmt"
	"io/ioutil"
	"net"
	"os"
	"strconv"
	"strings"
	"time"
)

var table map[int][]string

func readStdin(ch chan string) {
	stdinReader := bufio.NewReader(os.Stdin)
	for {
		o, err := stdinReader.ReadString('\n')
		if err != nil {
			fmt.Println(err)
			close(ch)
			return
		}
		ch <- o[:len(o)-1]
	}
}

// read and do nothing
func readTcp(conn net.Conn) {
	connReader := bufio.NewReader(conn)
	for {
		c, err := connReader.ReadString('\n')
		if err != nil {
			return
		}
		fmt.Printf("Read: %v", c)
	}
}

func handleConn(conn net.Conn) {

	stdinChan := make(chan string)
	go readStdin(stdinChan)
	go readTcp(conn)

	// read cmd and write to tcp
	for input := range stdinChan {
		val, err := strconv.Atoi(input)
		if err != nil {
			fmt.Println(err)
			continue
		}

		for _, v := range table[val] {
			// aa00058003%06x0100170607%02d%02d%02d0000\r\n
			t := time.Now()
			s := fmt.Sprintf("aa00%v0100%02d%02d%02d%02d%02d%02d0000\r\n",
				v, t.Year()%100, t.Month(), t.Day(), t.Hour(), t.Minute(), t.Second())
			_, err := conn.Write([]byte(s))
			if err != nil {
				conn.Close()
				close(stdinChan)
				return
			}
		}
	}
}

func main() {
	table = make(map[int][]string)

	args := os.Args
	if len(args) != 2 {
		panic("no file name")
	}

	dat, err := ioutil.ReadFile(args[1])
	if err != nil {
		panic(err)
	}

	rows := strings.Split(string(dat), "\n")
	for _, row := range rows {
		cells := strings.Split(row, " ")
		if len(cells) != 2 {
			break
		}
		id, _ := strconv.Atoi(cells[0])
		if table[id] == nil {
			table[id] = make([]string, 0, 10)
		}
		table[id] = append(table[id], cells[1])
	}

	ln, err := net.Listen("tcp", ":8000")
	if err != nil {
		panic(err)
	}

	for {
		conn, err := ln.Accept()
		if err != nil {
			fmt.Println(err)
			continue
		}

		go handleConn(conn)
	}
}
