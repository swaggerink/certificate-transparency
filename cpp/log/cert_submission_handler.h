#ifndef CERT_SUBMISSION_HANDLER_H
#define CERT_SUBMISSION_HANDLER_H

#include <string>

#include "base/macros.h"
#include "log/cert_checker.h"
#include "proto/ct.pb.h"
#include "proto/serializer.h"

// Parse incoming submissions, do preliminary sanity checks and pass them
// through cert checker.
// Prepare for signing by parsing the input into an appropriate
// log entry structure.
class CertSubmissionHandler {
 public:
  // Does not take ownership of the cert_checker.
  explicit CertSubmissionHandler(ct::CertChecker *cert_checker);
  ~CertSubmissionHandler() {}

  enum SubmitResult {
    OK,
    INVALID_TYPE,
    EMPTY_SUBMISSION,
    SUBMISSION_TOO_LONG,
    INVALID_PEM_ENCODED_CHAIN,
    INVALID_CERTIFICATE_CHAIN,
    PRECERT_CHAIN_NOT_WELL_FORMED,
    UNKNOWN_ROOT,
    INTERNAL_ERROR,
  };

  // These may change |chain|.
  SubmitResult ProcessX509Submission(ct::CertChain *chain,
                                     ct::LogEntry *entry);
  SubmitResult ProcessPreCertSubmission(ct::PreCertChain *chain,
					ct::LogEntry *entry);

  // For clients, to reconstruct the bytestring under the signature
  // from the observed chain. Does not check whether the entry
  // has valid format (i.e., does not check length limits).
  static bool X509ChainToEntry(const ct::CertChain &chain,
                               ct::LogEntry *entry);

  const std::multimap<std::string, const ct::Cert *> &GetRoots() const {
    return cert_checker_->GetTrustedCertificates();
  }

 private:
  static bool SerializedTbs(const ct::Cert &cert, std::string *result);
  static SubmitResult GetVerifyError(ct::CertChecker::CertVerifyResult result);

  ct::CertChecker *cert_checker_;

  DISALLOW_COPY_AND_ASSIGN(CertSubmissionHandler);
};
#endif
