#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

void process_stream(const char *filename, int fd) {
    struct stat file_stat;

    // Use fstat to get information about the file descriptor
    if (fstat(fd, &file_stat) == -1) {
        perror("fstat");
        return;
    }

    // Determine the type of the stream
    const char *stream_type;
    if (S_ISREG(file_stat.st_mode)) {
        stream_type = "is a regular file";
    } else if (S_ISCHR(file_stat.st_mode)) {
        stream_type = "is a character device";
    } else if (S_ISFIFO(file_stat.st_mode)) {
        stream_type = "is a pipe";
    } else {
        stream_type = "is something else";
    }

    // Output the information to stderr
    fprintf(stderr, "%s %s\n", filename, stream_type);

    // Process the content and copy it to stdout
    char buffer[4096];
    ssize_t bytesRead;

    while ((bytesRead = read(fd, buffer, sizeof(buffer))) > 0) {
        if (write(STDOUT_FILENO, buffer, bytesRead) != bytesRead) {
            perror("write");
            return;
        }
    }

    if (bytesRead == -1) {
        perror("read");
    }
}

int main(int argc, char *argv[]) {
    // Process each command line argument
    for (int i = 1; i < argc; ++i) {
        const char *filename = argv[i];

        // Check if the argument is '-'
        if (filename[0] == '-' && filename[1] == '\0') {
            // Process standard input
            process_stream("standard input", STDIN_FILENO);
        } else {
            // Process file
            int fd = open(filename, O_RDONLY);

            if (fd == -1) {
                perror("open");
                return 1;
            }

            process_stream(filename, fd);

            // Close the file descriptor
            close(fd);
        }
    }

    return 0;
}