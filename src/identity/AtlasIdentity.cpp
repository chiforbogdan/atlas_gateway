#include "AtlasIdentity.h"
#include "../logger/AtlasLogger.h"
#include <fstream>
#include <experimental/filesystem>

namespace fs = std::experimental::filesystem; 

namespace atlas {

namespace {

    const char *ATLAS_IDENTITY_FILE   = "atlas_gateway.identity";
    const char *ATLAS_PSK_FILE        = "atlas_gateway.psk";
    const int ATLAS_IDENTITY_LEN = 64;
    const int ATLAS_PSK_LEN      = 64;

    static char identity[ATLAS_IDENTITY_LEN + 1];
    static char psk[ATLAS_PSK_LEN + 1];

} // anonymous namespace

AtlasIdentity &AtlasIdentity::getInstance()
{
    static AtlasIdentity instance;

    return instance;
}

bool AtlasIdentity::generateIdentity()
{
    std::fstream fd;
    uuid_t uuid;

    ATLAS_LOGGER_DEBUG("Generate identity...");

    fd.open(ATLAS_IDENTITY_FILE, std::fstream::in | std::fstream::out | std::fstream::trunc);
    if (!fd.is_open()) {
        ATLAS_LOGGER_DEBUG("Error when opening the identity file");
        return false;
    }
    fs::path file = ATLAS_IDENTITY_FILE;
    fs::permissions(file, fs::perms::owner_read | fs::perms::owner_write | fs::perms::group_read | fs::perms::others_read);

   /* Generate identity */
    uuid_generate(uuid);
    uuid_unparse_lower(uuid, identity);
    identity_ = (char*)malloc(ATLAS_IDENTITY_LEN + 1);
    strncpy(identity_, identity, ATLAS_IDENTITY_LEN + 1);
    fd << identity;
    fd.close();

    ATLAS_LOGGER_DEBUG("Identity was generated");

    return true;
}
void AtlasIdentity::to_base64(const uint8_t *in, size_t in_len, char *out, size_t out_len)
{
    BIO *bmem, *b64;
    BUF_MEM *bptr;

    b64 = BIO_new(BIO_f_base64());
    BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL);
    
    bmem = BIO_new(BIO_s_mem());
    b64 = BIO_push(b64, bmem);
    BIO_write(b64, in, in_len);
    BIO_flush(b64);
    BIO_get_mem_ptr(b64, &bptr);

    memcpy(out, bptr->data,
           out_len > bptr->length ? bptr->length : out_len);

    BIO_free_all(b64);
}

bool AtlasIdentity::generatePsk()
{
    std::fstream fd;
    uint8_t rand_bytes[ATLAS_PSK_LEN/2];

    ATLAS_LOGGER_DEBUG("Generate PSK...");

    fd.open(ATLAS_PSK_FILE, std::fstream::in | std::fstream::out | std::fstream::trunc);
    if (!fd.is_open()) {
        ATLAS_LOGGER_DEBUG("Error when opening the PSK file");
        return false;
    }
    fs::path file = ATLAS_PSK_FILE;
    fs::permissions(file, fs::perms::owner_read | fs::perms::owner_write | fs::perms::group_read | fs::perms::others_read);

    /* Generate PSK */
    if (RAND_bytes(rand_bytes, sizeof(rand_bytes)) != 1) {
        fd.close();
	return false;
    }
    memset(psk, 0, sizeof(psk));
    to_base64(rand_bytes, sizeof(rand_bytes), psk, sizeof(psk) - 1);
    psk_ = (char*)malloc(ATLAS_PSK_LEN + 1);
    strncpy(psk_, psk, ATLAS_PSK_LEN + 1);
    fd << psk;
    fd.close();

    return true;
}

bool AtlasIdentity::initIdentity()
{
    bool status = false;
    std::ifstream fd;
    int len;

    ATLAS_LOGGER_DEBUG("Identity init...");

    /* Get identity */
    fd.open(ATLAS_IDENTITY_FILE, std::ios::binary | std::ios::ate);
    if (!fd.is_open()) {
        ATLAS_LOGGER_DEBUG("Identity file does not exist, generating an identity...");
        status = generateIdentity();
	if (status != true) {
            status = false;
            goto ERR;
	}
    } else {
        memset(identity, 0, sizeof(identity));
        fd >> identity;
        len = fd.tellg();
        fd.close();
        if (len <= 0) {
            ATLAS_LOGGER_ERROR("Error encountered when reading from the identity file");
            status = false;
            goto ERR;
        }
    }

    /* Get PSK */
    fd.open(ATLAS_PSK_FILE, std::ios::binary | std::ios::ate);
    if (!fd.is_open()) {
        ATLAS_LOGGER_DEBUG("PSK file does not exist, generating a PSK...");
        status = generatePsk();
        if (status != true) {
	    status = false;
            goto ERR;
	}	
    } else {
        memset(psk, 0, sizeof(psk));
        fd >> psk;
        len = fd.tellg();
        fd.close();
        if (len <= 0) {
            ATLAS_LOGGER_ERROR("Error encountered when reading from the PSK file");
            status = false;
            goto ERR;
        }
    }

    return true;

ERR:
    remove(ATLAS_IDENTITY_FILE);
    remove(ATLAS_PSK_FILE);

    return status;
}

} // namespace atlas
