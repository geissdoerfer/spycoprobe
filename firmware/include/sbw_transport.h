#ifndef __SBW_TRANSPORT_H_
#define __SBW_TRANSPORT_H_

#include <stdbool.h>

typedef struct {
  int sbwtck;
  int sbwtdio;
  int sbwdir;
} sbw_pins_t;

/* TMS low, TDI low */
void tmsl_tdil(void);
/* TMS high, TDI low */
void tmsh_tdil(void);
/* TMS low, TDI high */
void tmsl_tdih(void);
/* TMS high, TDI high */
void tmsh_tdih(void);

/* SBW transfer with TMS low, TDI high. Returns TDO. */
bool tmsl_tdih_tdo_rd(void);
/* SBW transfer with TMS low, TDI low. Returns TDO. */
bool tmsl_tdil_tdo_rd(void);
/* SBW transfer with TMS high, TDI high. Returns TDO. */
bool tmsh_tdih_tdo_rd(void);
/* SBW transfer with TMS high, TDI low. Returns TDO. */
bool tmsh_tdil_tdo_rd(void);

/* Clears JTAG TCLK signal via SBW */
void clr_tclk_sbw(void);
/* Sets JTAG TCLK signal via SBW */
void set_tclk_sbw(void);
/* Returns internal state of JTAG TCLK signal */
bool get_tclk(void);

/* Wrapper for setting SBWTDIO pin */
void set_sbwtdio(bool state);
/* Wrapper for setting SBWTCK pin */
void set_sbwtck(bool state);

int sbw_transport_setup(sbw_pins_t *sbw_pins);
int sbw_transport_stop(void);
int sbw_transport_start(void);

#endif /* __SBW_TRANSPORT_H_ */
