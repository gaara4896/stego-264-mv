/*
 * Copyright (c) 2012 Stefano Sabatini
 * Copyright (c) 2014 Clément Bœsch
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <libavutil/motion_vector.h>
#include <libavformat/avformat.h>
#include <getopt.h>
#include <stdbool.h>
#include "stegolib/stego_connector.h"

static AVFormatContext *fmt_ctx = NULL;
static AVCodecContext *video_dec_ctx = NULL;
static AVStream *video_stream = NULL;
static const char *video_file = NULL;

static int video_stream_idx = -1;
static AVFrame *frame = NULL;
static AVPacket pkt;
static int video_frame_count = 0;

static int decode_packet(int *got_frame, int cached) {
    int decoded = pkt.size;

    *got_frame = 0;

    if (pkt.stream_index == video_stream_idx) {
        int ret = avcodec_decode_video2(video_dec_ctx, frame, got_frame, &pkt);
        if (ret < 0) {
            fprintf(stderr, "Error decoding video frame (%s)\n", av_err2str(ret));
            return ret;
        }

        if (*got_frame) {
            int i;
            AVFrameSideData *sd;

            video_frame_count++;
            sd = av_frame_get_side_data(frame, AV_FRAME_DATA_MOTION_VECTORS);
            /*if (sd) {
                const AVMotionVector *mvs = (const AVMotionVector *)sd->data;
                for (i = 0; i < sd->size / sizeof(*mvs); i++) {
                    const AVMotionVector *mv = &mvs[i];
                    printf("%d,%2d,%2d,%2d,%4d,%4d,%4d,%4d,0x%"PRIx64"\n",
                            video_frame_count, mv->source,
                            mv->w, mv->h, mv->src_x, mv->src_y,
                            mv->dst_x, mv->dst_y, mv->flags);
                }
            }*/
        }
    }

    return decoded;
}

static int open_codec_context(int *stream_idx,
                              AVFormatContext *fmt_ctx, enum AVMediaType type) {
    int ret;
    AVStream *st;
    AVCodecContext *dec_ctx = NULL;
    AVCodec *dec = NULL;
    AVDictionary *opts = NULL;

    ret = av_find_best_stream(fmt_ctx, type, -1, -1, NULL, 0);
    if (ret < 0) {
        fprintf(stderr, "Could not find %s stream in input file '%s'\n",
                av_get_media_type_string(type), video_file);
        return ret;
    } else {
        *stream_idx = ret;
        st = fmt_ctx->streams[*stream_idx];

        /* find decoder for the stream */
        dec_ctx = st->codec;
        dec = avcodec_find_decoder(dec_ctx->codec_id);
        if (!dec) {
            fprintf(stderr, "Failed to find %s codec\n",
                    av_get_media_type_string(type));
            return AVERROR(EINVAL);
        }

        /* Init the video decoder */
        av_dict_set(&opts, "flags2", "+export_mvs", 0);
        if ((ret = avcodec_open2(dec_ctx, dec, &opts)) < 0) {
            fprintf(stderr, "Failed to open %s codec\n",
                    av_get_media_type_string(type));
            return ret;
        }
    }

    return 0;
}

bool is_supported_algorithm(const char *algname) {
    return strcmp(algname, "hidenseek") == 0
           || strcmp(algname, "rand-hidenseek") == 0
           || strcmp(algname, "dumpmvs") == 0
           || strcmp(algname, "mvsteg") == 0;
}

int main(int argc, char **argv) {
    int ret = 0, got_frame;

    static int encryptFlag = 0;
    char* algorithm = NULL;
    char* password = NULL;
    char* endPtr = NULL;
    uint32_t capacity = 0;
    uint32_t fileSize = 0;

    static struct option long_options[] =
    {
        {"decrypt", no_argument, &encryptFlag, 1},
        {"algorithm", required_argument, 0, 'a'},
        {"password", required_argument, 0, 'p'},
        {"capacity", required_argument, 0, 'c'},
        {"file-size", required_argument, 0, 'f'},
        {"help", no_argument, 0, 'h'}
    };

    int option_index = -1, c;
    while((c = getopt_long(argc, argv, "a:p:c:f:h", long_options, &option_index)) != -1) {
        switch(c) {
            case 0:
                // Flag handled
                break;
            case 'a':
                if(!optarg || is_supported_algorithm(optarg) == 0) {
                    av_log(NULL, AV_LOG_ERROR, "No or unsupported algorithm provided (%s).\n", optarg);
                    return 1;
                }
                algorithm = optarg;
                break;
            case 'p':
                if(!optarg) {
                    av_log(NULL, AV_LOG_ERROR, "-p/--password requires seed data (a string) as an argument.\n");
                    return 1;
                }
                password = optarg;
                break;
            case 'c':
                if(!optarg) {
                    av_log(NULL, AV_LOG_ERROR, "-c/--capacity requires an integer as an argument.\n");
                    return 1;
                }
                capacity = atoi(optarg); // TODO: fix
                break;
            case 'f':
                if(!optarg) {
                    av_log(NULL, AV_LOG_ERROR, "-f/--file-size output file size in bytes as an argument.\n");
                    return 1;
                }
                fileSize = atoi(optarg); // TODO: fix
                break;
            case 'h':
                // Print some useful help.
                return 0;
            default:
                av_log(NULL, AV_LOG_ERROR, "Unknown option provided: %c\n", c);
                return 1;
        }
    }

    char* video_file = argv[optind++];
    char* data_out_file = argv[optind++];
    if(optind != argc) {
        av_log(NULL, AV_LOG_ERROR, "Incorrect number of arguments provided.\n"
                "Usage:\n"
                "%s [options] <input_video> <output_file>\n", argv[0]);
        return 1;
    }

    if(!algorithm) {
        av_log(NULL, AV_LOG_INFO, "Algorithm not specified. Using 'mvsteg' as a default.\n");
        algorithm = "mvsteg";
    }

    if (encryptFlag && !password){
        av_log(NULL, AV_LOG_ERROR, "You must provide a password if you want to use crypto. Use -p/--password. \n");
        return 1;
    }

    av_log(NULL, AV_LOG_INFO, "Video file: %s\n", video_file);
    av_log(NULL, AV_LOG_INFO, "Output file: %s\n", data_out_file);
    av_log(NULL, AV_LOG_INFO, "Algorithm: %s\n", algorithm);
    av_log(NULL, AV_LOG_INFO, "Crypto: %s\n", encryptFlag? "ON" : "OFF");
    if(strcmp(algorithm, "rand-hideseek") == 0) {
        if(!password) {
            av_log(NULL, AV_LOG_ERROR, "You must provide a password for this algorithm. Use -p/--password. \n");
            return 1;
        }
        if(capacity == 0) {
            av_log(NULL, AV_LOG_ERROR, "You must provide capacity parameter that encoder used. Use -c/--capacity. \n");
            return 1;
        }
        if(fileSize == 0) {
            av_log(NULL, AV_LOG_ERROR, "You must provide the resulting file size. Use -f/--file-size. \n");
            return 1;
        }
        
        av_log(NULL, AV_LOG_INFO, "Password: *SET*");
        av_log(NULL, AV_LOG_INFO, "Capacity: %d\n", capacity);
    }
    if(fileSize != 0) {
        av_log(NULL, AV_LOG_INFO, "Output file size: %d\n", fileSize);
    }

    if (stego_init_algorithm(algorithm)) {
        fprintf(stderr, "Init algorithm fail.\n");
        return 1;
    }
    struct algoptions {
        uint32_t byteCapacity;
        uint32_t fileSize;
    };
    struct algoptions algparams = {capacity, fileSize};
    stego_params p = {
        data_out_file, encryptFlag? STEGO_ENABLE_ENCRYPTION : STEGO_NO_PARAMS, password, &algparams
    };
    stego_init_decoder(&p);

    av_register_all();

    if (avformat_open_input(&fmt_ctx, video_file, NULL, NULL) < 0) {
        fprintf(stderr, "Could not open source file %s\n", video_file);
        exit(1);
    }

    if (avformat_find_stream_info(fmt_ctx, NULL) < 0) {
        fprintf(stderr, "Could not find stream information\n");
        exit(1);
    }

    if (open_codec_context(&video_stream_idx, fmt_ctx, AVMEDIA_TYPE_VIDEO) >= 0) {
        video_stream = fmt_ctx->streams[video_stream_idx];
        video_dec_ctx = video_stream->codec;
    }

    av_dump_format(fmt_ctx, 0, video_file, 0);

    if (!video_stream) {
        fprintf(stderr, "Could not find video stream in the input, aborting\n");
        ret = 1;
        goto end;
    }

    frame = av_frame_alloc();
    if (!frame) {
        fprintf(stderr, "Could not allocate frame\n");
        ret = AVERROR(ENOMEM);
        goto end;
    }

    /* initialize packet, set data to NULL, let the demuxer fill it */
    av_init_packet(&pkt);
    pkt.data = NULL;
    pkt.size = 0;

    /* read frames from the file */
    while (av_read_frame(fmt_ctx, &pkt) >= 0) {
        AVPacket orig_pkt = pkt;
        do {
            ret = decode_packet(&got_frame, 0);
            if (ret < 0)
                break;
            pkt.data += ret;
            pkt.size -= ret;
        } while (pkt.size > 0);
        av_free_packet(&orig_pkt);
    }

    /* flush cached frames */
    pkt.data = NULL;
    pkt.size = 0;
    do {
        decode_packet(&got_frame, 1);
    } while (got_frame);

    end:
    avcodec_close(video_dec_ctx);
    avformat_close_input(&fmt_ctx);
    av_frame_free(&frame);
    if (ret < 0) return ret;

    stego_result res = stego_finalise();
    fprintf(stderr, "Bytes processed: %d\n", res.bytes_processed);

    return res.error;
}