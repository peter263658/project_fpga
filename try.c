#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>


unsigned char transmission_table (unsigned char , unsigned char);
void I010(unsigned char * buffer, FILE * output_file);
void I081(unsigned char * buffer, FILE * output_file);
void I024(unsigned char * buffer, FILE * output_file);
void READ_ASCII(unsigned char ascii_letter, FILE* output_file);
void READ_PACK_BCD (unsigned char bcd_raw_number, FILE * output_file, bool flag);



int main() {

    unsigned char buffer[1<<16];
    const char *file_name_I081 = "output_I081.csv";
    const char *file_name_I024 = "output_I024.csv"; 
    const char *file_name_I010 = "output_I010.csv";   
    FILE * file_pointer = fopen("MarketDataLog_20210709.txt", "rb");                 // rb: read binary
    FILE * output_file_I081 = fopen(file_name_I081, "w+");    // output file
    FILE * output_file_I024 = fopen(file_name_I024, "w+");    // output file
    FILE * output_file_I010 = fopen(file_name_I010, "w+");    // output file
    unsigned long long counter_i024 = 0;

    if (!output_file_I024 || !output_file_I081 || !output_file_I010) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    fread(buffer, 24, 1, file_pointer);
    for (unsigned int number = 1; number <= 50000000; number++) {
        fread(buffer, 58, 1, file_pointer);
        unsigned short udp_length = buffer[54] << 8 | buffer[55] - 8;
        fread(buffer, udp_length, 1, file_pointer);

        // printf(
        // // "Order number : %llu \n"
        // "Serial Number : %u\n"
        // "UDP Length : %u\n"
        // //"Index\n\    "
        // "      0   1   2   3   4   5   6   7    8   9   a   b   c   d   e   f"
        // , number, udp_length);
        // for (unsigned short i = 0; i < udp_length; i++) {
        //     if (i % 16 == 0)
        //         printf("\n%04x", i);
        //     if (i % 8 == 0)
        //         printf(" ");
        //     printf("%02x  ", buffer[i]);
        // }
        // printf("\n\n\n");
        // judge the packeage
        if (transmission_table(buffer[1], buffer[2]) == 0x51) {
            I081(buffer, output_file_I081);         // unpack the I081
            fputc('\n', output_file_I081);
        }

        else if (transmission_table(buffer[1], buffer[2]) == 0x18) {

            // printf("Order number : %llu \n"
            // "Serial Number : %u\n"
            // "UDP Length : %u\n"
            // //"Index\n\    "
            // "      0   1   2   3   4   5   6   7    8   9   a   b   c   d   e   f"
            // , counter_i024, number, udp_length);
            // for (unsigned short i = 0; i < udp_length; i++) {
            //     if (i % 16 == 0)
            //         printf("\n%04x", i);
            //     if (i % 8 == 0)
            //         printf(" ");
            //     printf("%02x  ", buffer[i]);
            // }
            // printf("\n\n\n");

            I024(buffer, output_file_I024);         // unpack the I024
            fputc('\n', output_file_I024);
        }

        else if (transmission_table(buffer[1], buffer[2]) == 0x0A) {
            I010(buffer, output_file_I010);         // unpack the I010
            fputc('\n', output_file_I010);
        }

        else if (transmission_table(buffer[1], buffer[2]) == 0) {
            continue;
        }
        // check the data

        // printf("Serial Number : %u\n"
        //     "UDP Length : %u\n"
        //     //"Index\n\    "
        //     "      0   1   2   3   4   5   6   7    8   9   a   b   c   d   e   f"
        //     , number, udp_length);
        // for (unsigned short i = 0; i < udp_length; i++) {
        //     if (i % 16 == 0)
        //         printf("\n%04x", i);
        //     if (i % 8 == 0)
        //         printf(" ");
        //     printf("%02x  ", buffer[i]);
        // }
        // printf("\n\n\n");
    }

    fclose(file_pointer);
    fclose(output_file_I081);
    fclose(output_file_I024);
    fclose(output_file_I010);
    return 0;

}
unsigned char transmission_table (unsigned char transmission_code, unsigned char message_kind) {
    if(transmission_code == '1' && message_kind =='1'){
        // printf("decode I010, function will be built\n");
        return 0x0A;
    }
    else if (transmission_code == '2') {
        if (message_kind == 'A') {
            printf("decode I081, function will be built\n");
            return 0x51;
            //our target, next step is that analysis the data
        }
        else if (message_kind == 'D') {
            printf("decode I024, function will be built\n"); 
            return 0x18;
        }
    //     else
    //         printf("we don't care\n");
    }
    else
        return 0;
}

// unpack PACK BCD and write to the respond output file
void READ_PACK_BCD (unsigned char bcd_raw_number, FILE * output_file, bool flag) {                            // 1 BCD to 2 number
    unsigned char temp10, temp1;
    temp10 = (bcd_raw_number >> 4 |0x30);
    temp1 = ((bcd_raw_number & 0x0f) | 0x30);
    fputc(temp10, output_file);
    fputc(temp1, output_file);
    //fwrite(temp, sizeof(unsigned char), 1, output_file);
    if (flag) {
        fputc(',', output_file);          // next column
    }
    return;
}

// unpack normal letter (ascii) and write to the respond output fileMarketDataLog_20210709.txt
void READ_ASCII(unsigned char ascii_letter, FILE * output_file) {
    fputc(ascii_letter, output_file);
    //fwrite(ascii_letter, 1, 1, output_file);
    fputc(',', output_file); 
    return;
}

void I010 (unsigned char * buffer, FILE * output_file) {
    for (int count = 0; count < 10; count++) {                // PROD_ID[10]
        fputc(buffer[count + 19], output_file);                  
    } fputc(',', output_file); 
    for(int count = 0; count < 5; count++){                   //RISE_LIMIT_PRICE1[5]
        READ_PACK_BCD(buffer[29 + count], output_file, false);                                    
    } fputc(',', output_file);
    for(int count = 0; count < 5; count++){                   //REFERENCE_PRICE[5]
        READ_PACK_BCD(buffer[34 + count], output_file, false);                                    
    } fputc(',', output_file);
    for(int count = 0; count < 5; count++){                   //FALL_LIMIT_PRICE1[5]
            READ_PACK_BCD(buffer[39 + count], output_file, false);                                    
    } fputc(',', output_file);
    for(int count = 0; count < 5; count++){                   //RISE_LIMIT_PRICE2[5]
        READ_PACK_BCD(buffer[44 + count], output_file, false);                                    
    } fputc(',', output_file);
    for(int count = 0; count < 5; count++){                   //FALL_LIMIT_PRICE2[5]
        READ_PACK_BCD(buffer[49 + count], output_file, false);                                    
    } fputc(',', output_file);
    for(int count = 0; count < 5; count++){                   //RISE_LIMIT_PRICE3[5]
        READ_PACK_BCD(buffer[54 + count], output_file, false);                                    
    } fputc(',', output_file);
    for(int count = 0; count < 5; count++){                   //FALL_LIMIT_PRICE3[5]
        READ_PACK_BCD(buffer[59 + count], output_file, false);                                    
    } fputc(',', output_file);
    return;
    READ_ASCII(buffer[64], output_file);                      //PROD_KIND
    READ_PACK_BCD(buffer[65], output_file, true);             //DECIMAL_LOCATOR
    READ_PACK_BCD(buffer[66], output_file, true);             //STRIKE_PRICE_DECIMAL_LOCATOR
    for(int count = 0; count < 4; count++){                   //BEGIN_DATE:YYYYMMDD
        READ_PACK_BCD(buffer[67 + count], output_file, false);                                    
    } fputc(',', output_file);
    for(int count = 0; count < 4; count++){                   //END_DATE:YYYYMMDD
        READ_PACK_BCD(buffer[71 + count], output_file, false);                               
    } fputc(',', output_file);
    READ_PACK_BCD(buffer[75], output_file, true);             //FLOW_GROUP
    for(int count = 0; count < 4; count++){                   //DELIVERY_DATE:YYYYMMDD
        READ_PACK_BCD(buffer[76 + count], output_file, false);                               
    } fputc(',', output_file);
    READ_ASCII(buffer[80], output_file);                      //DYNAMIC_BANDING 
}


void I081 (unsigned char * buffer, FILE * output_file) {
    unsigned char no_md_entries = (buffer[44] >> 4) * 10 + (buffer[44] & 0x0f);
    // printf("no md entries is %02x\n", buffer[44]);
    // printf("no md entries is %d\n", no_md_entries);
    unsigned short i = -13;
    for (int count = 0; count < 20; count++) {                 // PROD_ID[20]
        fputc(buffer[count + 19], output_file);
        //fwrite(buffer[count + 19], 1, 1, output_file);                      
    } fputc(',', output_file); 
    for (int count = 0; count < 5; count++) {                  // PROD_MSG_SEQ[5]
        READ_PACK_BCD(buffer[39 + count], output_file, false);                                    
    }   fputc(',', output_file); 
    while(no_md_entries--){        
        i += 13;
        READ_ASCII(buffer[45 + i], output_file);               // MD_UPDATE_ACTION
        READ_ASCII(buffer[46 + i], output_file);               // MD_ENTRY_TYPE
        READ_ASCII(buffer[47 + i], output_file);               // SIGN
        for (int count = 0; count < 5; count++) {              // // MD_ENTRY_PX[5]
            READ_PACK_BCD(buffer[48 + i + count], output_file, false);
        }   fputc(',', output_file);  
        for (int count = 0; count < 4; count++) {              // MD_ENTRY_SIZE[4]
            READ_PACK_BCD(buffer[count + i + 53], output_file, false);
        }   fputc(',', output_file);                                                    
        READ_PACK_BCD(buffer[57 + i], output_file, true);      // MD_PRICE_LEVEL
    }
    // READ_ASCII(buffer[58 + i], output_file);                // CHECK_SUM
    // for (int count = 0; count < 2; count++) {               // PROD_ID[20]
    //     fputc(buffer[59 + i +count], output_file);
    //     //fwrite(buffer[59 + i + count], 1, 1, output_file);                      
    // } fputc(',', output_file);                              // TERMINAL_CODE[2]
    return;
}

void I024 (unsigned char * buffer, FILE * output_file) {
    // if (buffer[62] & 0x80 == 0x80) {                         // judge whether first package
        for (int count = 0; count < 20; count++) {              // PROD_ID[20]
            fputc(buffer[count + 19], output_file);                    
        } fputc(',', output_file);
        for (int count = 0; count < 5; count++) {               // PROD_MSG_SEQ[5]
            READ_PACK_BCD(buffer[39 + count], output_file, false);                                    
        }   fputc(',', output_file);
        READ_ASCII(buffer[44], output_file);                    // CALVULATED_FLAG
        for (int count = 0; count < 6; count++) {               // MATCH_TIME HOUR:MINUTE:SECOND:MSECOND:USECOND
            READ_PACK_BCD(buffer[count + 45], output_file, false);                    
        } fputc(',', output_file);
        if (buffer[51]) 
            fputc('-', output_file);
        else                                                    //SIGN(PRICE) 
            fputc('+', output_file);
        fputc(',', output_file);
        for (int count = 0; count < 5; count++) {               // FIRST_MATCH_PRICE[5]
            READ_PACK_BCD(buffer[52 + count], output_file, false);                                    
        }   fputc(',', output_file);
        for (int count = 0; count < 4; count++) {               // FIRST_MATCH_QTY[4]
            READ_PACK_BCD(buffer[57 + count], output_file, false);                                    
        }   fputc(',', output_file);
        READ_ASCII(buffer[61], output_file);                    // MATCH_DISPLAY_ITEM
        // MATCH_DATA and so on 
    // }
    unsigned char match_data_item_number;
    unsigned char index = 63;
    match_data_item_number = (buffer[62] & 0x7F);
    printf("match number is %u\n", match_data_item_number);
    // match_data_item_number--;
    while (match_data_item_number--) {
        if (buffer[index++]) 
            fputc('-', output_file);
        else                                                   //SIGN 
            fputc('+', output_file);
        fputc(',', output_file);
        for (int count = 0; count < 5; count++) {              // Match-Price
            READ_PACK_BCD(buffer[index++], output_file, false);
        } fputc(',', output_file);
        for (int count = 0; count < 2; count++) {              // Match-qty
            READ_PACK_BCD(buffer[index++], output_file, false);
        } fputc(',', output_file);   
    }
    for (int count = 0; count < 4; count++) {                  // Match-total-qty
        READ_PACK_BCD(buffer[index++], output_file, false);
    } fputc(',', output_file);
    for (int count = 0; count < 4; count++) {                  // match-buy-cnt
        READ_PACK_BCD(buffer[index++], output_file, false);
    } fputc(',', output_file);
    for (int count = 0; count < 4; count++) {                  // match-sell-cnt
        READ_PACK_BCD(buffer[index++], output_file, false);
    } fputc(',', output_file);
}


// ****target**** 
// analysis I081 & I024 (so long)
// find the imformation of price in I010
// decode the pack bcd
// write to file


//I081
// unsigned char PROD_ID[20];//from buffer[19]
// unsigned char PROD_MSG_SEQ[5];  //READ_PACK_BCD
// unsigned char NO_MD_ENTRIES;    //READ_PACK_BCD
// unsigned char MD_UPDATE_ACTION; //0:NEW  1:CHANGE  2:DELETE  5:OVERLAY
// unsigned char MD_ENTRY_TYPE; //0:BUY 1:SELL E:衍生買 F:衍生賣
// unsigned char SIGN;
// unsigned char MD_ENTRY_PX[5];//READ_PACK_BCD
// unsigned char MD_ENTRY_SIZE[4];//READ_PACK_BCD
// unsigned char MD_PRICE_LEVEL;//READ_PACK_BCD
// unsigned char CHECK_SUM;
// unsigned char TERMINAL_CODE[2];
