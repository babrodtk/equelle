
#ifndef EQUELLE_COLLOFVECTOR_HEADER_INCLUDED
#define EQUELLE_COLLOFVECTOR_HEADER_INCLUDED


#include <cuda.h>
#include <cuda_runtime.h>

#include <vector>

#include "CollOfScalar.hpp"
#include "equelleTypedefs.hpp"


namespace equelleCUDA {

    //! Collection of Vectors
    /*!
      This class is holds a collection of Vectors. In addition to the number of vectors
      (numVectors()) we need a dimension (dim()) to state how many elements each 
      vector in the collection have. Since all grids in Equelle is either 2D or 3D,
      2 and 3 are the only legal dimensions that can be passed to this class. We can 
      also talk about number of elements (numElements()) which returns the number of 
      Vectors times the dimension.

      The vector elements are stored in a private member variable of type CollOfScalar
      packed so that each vector is contiguous in memory, hence a CollOfVector with
      N vectors of 3 dimensions is stored  {1_x, 1_y, 1_z, 2_x, 2_y, 2_x,...,N_x, 
      N_y, N_z}, and the access to the raw data (data()) gives a pointer to this
      memory.

      There are two ways of launching CUDA kernels that do operations on a 
      CollOfVector. The first is by using one thread for each vector, 
      and the other one thread for each element. These block and grid sizes 
      are stored in a kernelSetup datatype available through vector_setup()
      and element_setup().
    */
    class CollOfVector
    {
    public:
	//! Default constructor
	CollOfVector();
	
	//! Allocating constructor
	/*!
	  Allocates device memory without initialization.
	  \param size The number of vectors in the collection
	  \param dim Dimension of each vector
	*/
	explicit CollOfVector(const int size, const int dim);

	//! Constructor from std::vector
	/*!
	  Used for easy testing. The std::vectors contains the vector elements,
	  not the vectors them selves. The size of the collection will therefore
	  be host.size()/dim.
	  \param host A host vector with {1_x, 1_y, 1_z, 2_x, 2_y, 2_x,...,N_x, 
	  N_y, N_z} for a 3 dimensional case. 
	  \param dim The dimension of the vectors stored in host.
	*/
	explicit CollOfVector(const std::vector<double>& host, const int dim);

	//! Copy constructor
	/*!
	  Allocates memory for a new CollOfVector and copies all data from coll 
	  over to the new variable.
	*/
	CollOfVector(const CollOfVector& coll);

	//! Copy assignment operator
	/*!
	  Overload the assignment operator to ensure correct behaviour when 
	  we assign a CollOfVector to a CollOfVector that is already initialized.
	*/
	CollOfVector& operator= (const CollOfVector& other);

	//! Destructor
	/*!
	  Needed in order to automatically call the base class destructor.
	*/
	~CollOfVector();
	


	//! Norm of the vectors in the collection
	/*!
	  Returns a collection of scalars equal to the norm of every vector in
	  the caller. The norm used here is the 2-norm equal to the square root of 
	  the sum of the squared elements of each vector.
	*/
	CollOfScalar norm() const;

	//! Pointer to the device memory block with all elements
	const double* data() const;
	//! Pointer to the device memory block with all elements
	double* data();


	// Temporary function to make everything work again
	int size() const;

	//! Dimension of vectors in the collection
	int dim() const;

	//! Number of vectors in the collection
	/*!
	  This function returns the number of vectors in the collection.
	  Not to be confused with size() which returns total number of 
	  elements in the collection.
	  size() = numVectors()*dim()
	*/
	int numVectors() const;

	//! Number of elements in the collection
	/*!
	  This function returns the number of elements in total in the 
	  collection of vectors. Hence, it returns numVectors()*dim
	*/
	int numElements() const;

	kernelSetup vector_setup() const;
	kernelSetup element_setup() const;
	

	//! Index operator
	/*!
	  Returns a collection of Scalars with the values from the index of each of the
	  vectors. myVector[1] will not return the second vector in the collection
	  but a collection of the second component from all the vectors.
	*/
	CollOfScalar operator[](const int index) const;
	
	// These functions are generated from the compiler, and we need therefore
	// to implement them.
	//CollOfScalar& col(const int c);
	//const CollOfScalar& col(const int c) const;
	CollOfScalar col(const int index) const;
	// The one implemented here makes a copy though...

    private:
	CollOfScalar elements_;
	const int dim_;
	//kernelSetup element_setup_; // Find this one as elements_.getKernelSetup()
	kernelSetup vector_setup_;

	// size_ from CollOfScalar is actually size_ * dim
	// block() and grid() will therefore be evaluated as one thread per double
    };

    //! Kernel for getting the index element of all vectors in a collection.
    /*!
      \param[out] out Collection Of Scalar where out[i] is the index element of
      vector number i in the collection of Vectors
      \param[in] vec Collection of Vectors as a double array of size size_out*dim
      \param[in] size_out Number of vectors in the collection and also the number 
      of elements in the output collection of scalars.
      \param[in] index The index we want to read from each vector
      \param[in] dim The dimension of the vectors in the collection.
    */
    __global__ void collOfVectorOperatorIndexKernel( double* out,
						     const double* vec,
						     const int size_out,
						     const int index,
						     const int dim);
	
    //! Kernel for computing the norm of vectors
    /*!
      Uses one thread for each vector to compute the given vectors norm.
      
      \param[out] out The output with the norm of each vector
      \param[in] vectors Array with vector elements so that each vector is 
      continously in memory. The size of this array is numVectors*dim.
      \param[in] numVectors Number of vectors given in input
      \param[in] dim Dimension of each vector.
    */
    __global__ void normKernel( double* out,
				const double* vectors,
				const int numVectors,
				const int dim);
    


    // --------------------- OPERATOR OVERLOADING -------------------------

    /*!
      Overloaded operator + for Collection of Vectors. Elementwise addition
      of all values stored in the collections.
      
      Works as a wrapper for the CUDA kernel which add collection of scalars.
    */
    CollOfVector operator+(const CollOfVector& lhs, const CollOfVector& rhs);

    /*!
      Overloaded operator - for Collection of Vectors. Elementwise subtraction
      of all values stored in the collection.

      Works as a wrapper for the CUDA kernel which subtract collection of scalars.
    */
    CollOfVector operator-(const CollOfVector& lhs, const CollOfVector& rhs);

} // namespace equelleCUDA


#endif // EQUELLE_COLLOFVECTOR_HEADER_INCLUDED