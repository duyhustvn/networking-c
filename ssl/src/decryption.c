/*
** This program is used to decrypt the content of certificate such as ssl certificate
 */

#include <stdio.h>
#include <string.h>
#include <err.h>

#include <openssl/bio.h>
#include <openssl/pem.h>
#include <openssl/x509.h>

void decode_ssl_content(BIO *cert_bio) {
  // remember to free cert_bio
   X509 *x509 = PEM_read_bio_X509(cert_bio, NULL, NULL, NULL);
   if (!x509) {
       fprintf(stderr, "Error reading certificate\n");
       return;
   }

    X509_print_fp(stdout, x509);
    X509_free(x509);
}

int main() {
    char *fileName = getenv("FILE");;
    if (!fileName) {
        errx(1, "ERROR: failed to load file from environment");
        return -1;
    }

    FILE *f = fopen(fileName, "r");
    if (!f) {
        fprintf(stderr, "Error reading pem file");
        return 1;
    }

    // Get the size of the file
    fseek(f, 0, SEEK_END);
    int file_size = ftell(f);
    rewind(f);


    // convert file content to BIO
    char *file_content = (char*)malloc(file_size + 1);
    if (!file_content) {
        fprintf(stderr, "Error malloc for file content");
        fclose(f);
        return 1;
    }

    // read content of file into buffer
    fread(file_content, 1, file_size, f);
    file_content[file_size] = '\0';

    fclose(f);

    BIO *mem_bio = BIO_new_mem_buf(file_content, file_size);
    if (!mem_bio) {
        fprintf(stderr, "Error creating mem_bio");
        free(file_content);
        return 1;
    }

    decode_ssl_content(mem_bio);

    // clean up
    BIO_free(mem_bio);
    free(file_content);

    return 0;
}
