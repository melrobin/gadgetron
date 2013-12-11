#include "GadgetIsmrmrdReadWrite.h"
#include "RemoveROOversamplingGadget.h"
#include "Gadgetron.h"
#include "hoNDFFT.h"

namespace Gadgetron{

int RemoveROOversamplingGadget
::process(GadgetContainerMessage<ISMRMRD::AcquisitionHeader>* m1,
	  GadgetContainerMessage< hoNDArray< std::complex<float> > >* m2)
{
  GadgetContainerMessage< hoNDArray< std::complex<float> > >* m3 
    = new GadgetContainerMessage< hoNDArray< std::complex<float> > >();

  if (!m3) {
    return GADGET_FAIL;
  }

  std::vector<size_t> data_out_dims = *m2->getObjectPtr()->get_dimensions();
  data_out_dims[0] = data_out_dims[0]/2;

  try{ m3->getObjectPtr()->create(&data_out_dims);}
  catch (std::runtime_error &err){
    GADGET_DEBUG_EXCEPTION(err,"Unable to create new data array for downsampled data\n");
    return GADGET_FAIL;
  }

  hoNDFFT<float>::instance()->ifft(m2->getObjectPtr(),0);
  
  std::complex<float>* data_in  = m2->getObjectPtr()->get_data_ptr();
  std::complex<float>* data_out = m3->getObjectPtr()->get_data_ptr();

  for (unsigned int c = 0; c < data_out_dims[1]; c++) {
    size_t offset_in = c*m2->getObjectPtr()->get_size(0) +  (m2->getObjectPtr()->get_size(0)-data_out_dims[0])/2;
    size_t offset_out = c*m3->getObjectPtr()->get_size(0);
    memcpy(data_out+offset_out,data_in+offset_in,data_out_dims[0]*sizeof(std::complex<float>));
  }

  hoNDFFT<float>::instance()->fft(m3->getObjectPtr(),0);
  
  m2->release(); //We are done with this data

  m1->cont(m3);
  m1->getObjectPtr()->number_of_samples = data_out_dims[0];
  m1->getObjectPtr()->center_sample /= 2;

  if (this->next()->putq(m1) == -1) {
    ACE_ERROR_RETURN( (LM_ERROR,
		       ACE_TEXT("%p\n"),
		       ACE_TEXT("RemoveROOversamplingGadget::process, passing data on to next gadget")),
		      GADGET_FAIL);
  }

  return GADGET_OK;
}


GADGET_FACTORY_DECLARE(RemoveROOversamplingGadget)
}